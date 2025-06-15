#include <winsock2.h>
#include <ws2tcpip.h>
#include <bcrypt.h>
#include <wincrypt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "crypt32.lib")

#define GUID_WEBSOCKET "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define WM_SOCKET (WM_USER + 1)
#define WM_PING_TIMER (WM_USER + 2)
#define MAX_CLIENTS 64
#define PING_INTERVAL 30000 // 30秒发送一次 Ping

// 客户端状态
typedef struct {
    SOCKET socket;
    char *fragment_buf;
    size_t fragment_len;
    int handshaked;
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;

// 使用 CNG 计算 SHA-1
int compute_sha1(const char *input, unsigned char *output, size_t *output_len) {
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;
    NTSTATUS status;
    DWORD hash_len, cbData;

    status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA1_ALGORITHM, NULL, 0);
    if (!BCRYPT_SUCCESS(status)) return -1;

    status = BCryptCreateHash(hAlg, &hHash, NULL, 0, NULL, 0, 0);
    if (!BCRYPT_SUCCESS(status)) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return -1;
    }

    status = BCryptHashData(hHash, (PUCHAR)input, (ULONG)strlen(input), 0);
    if (!BCRYPT_SUCCESS(status)) {
        BCryptDestroyHash(hHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return -1;
    }

    status = BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH, (PUCHAR)&hash_len, sizeof(hash_len), &cbData, 0);
    if (!BCRYPT_SUCCESS(status)) {
        BCryptDestroyHash(hHash);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return -1;
    }

    status = BCryptFinishHash(hHash, output, hash_len, 0);
    *output_len = hash_len;

    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);
    return BCRYPT_SUCCESS(status) ? 0 : -1;
}

// 计算 Sec-WebSocket-Accept
void compute_websocket_accept(const char *key, char *accept) {
    char concat[128];
    unsigned char sha1_result[20];
    size_t sha1_len;
    DWORD accept_len;

    snprintf(concat, sizeof(concat), "%s%s", key, GUID_WEBSOCKET);

    if (compute_sha1(concat, sha1_result, &sha1_len) == 0) {
        CryptBinaryToStringA(sha1_result, sha1_len, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &accept_len);
        CryptBinaryToStringA(sha1_result, sha1_len, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, accept, &accept_len);
    } else {
        accept[0] = '\0';
    }
}

// 发送 WebSocket 帧
void send_frame(SOCKET client, unsigned char opcode, const char *data, size_t data_len) {
    char *frame = (char *)malloc(data_len + 14); // 最大头部 14 字节
    if (!frame) return;

    frame[0] = 0x80 | opcode; // FIN=1
    size_t offset = 2;

    if (data_len <= 125) {
        frame[1] = (char)data_len;
    } else if (data_len <= 65535) {
        frame[1] = 126;
        frame[2] = (data_len >> 8) & 0xFF;
        frame[3] = data_len & 0xFF;
        offset += 2;
    } else {
        frame[1] = 127;
        for (int i = 7; i >= 0; i--) {
            frame[offset++] = (data_len >> (i * 8)) & 0xFF;
        }
    }

    memcpy(frame + offset, data, data_len);
    send(client, frame, offset + data_len, 0);
    free(frame);
}

// 解析 WebSocket 帧
int parse_frame(SOCKET client, char *buffer, size_t len, char **payload, size_t *payload_len, char **fragment_buf, size_t *fragment_len) {
    if (len < 2) return -1;

    unsigned char fin = buffer[0] & 0x80;
    unsigned char opcode = buffer[0] & 0x0F;
    unsigned char mask = buffer[1] & 0x80;
    *payload_len = buffer[1] & 0x7F;
    size_t offset = 2;

    // 处理扩展长度
    if (*payload_len == 126) {
        if (len < 4) return -1;
        *payload_len = (buffer[2] << 8) | buffer[3];
        offset += 2;
    } else if (*payload_len == 127) {
        if (len < 10) return -1;
        *payload_len = ((uint64_t)buffer[2] << 56) | ((uint64_t)buffer[3] << 48) |
                       ((uint64_t)buffer[4] << 40) | ((uint64_t)buffer[5] << 32) |
                       ((uint64_t)buffer[6] << 24) | ((uint64_t)buffer[7] << 16) |
                       ((uint64_t)buffer[8] << 8) | buffer[9];
        offset += 8;
    }

    // 处理掩码
    unsigned char mask_key[4] = {0};
    if (mask) {
        if (len < offset + 4) return -1;
        memcpy(mask_key, buffer + offset, 4);
        offset += 4;
    }

    if (len < offset + *payload_len) return -1;
    *payload = (char *)malloc(*payload_len + 1);
    if (!*payload) return -1;
    memcpy(*payload, buffer + offset, *payload_len);

    // 解码掩码
    if (mask) {
        for (size_t i = 0; i < *payload_len; i++) {
            (*payload)[i] ^= mask_key[i % 4];
        }
    }
    (*payload)[*payload_len] = '\0';

    // 处理关闭帧
    if (opcode == 0x8) { // Close
        uint16_t status_code = 0;
        if (*payload_len >= 2) {
            status_code = ((*payload)[0] << 8) | (*payload)[1];
            printf("Close frame received from %lld, status: %u\n", (long long)client, status_code);
        }
        send_frame(client, 0x8, *payload, *payload_len);
        free(*payload);
        return -2; // 指示关闭连接
    }

    // 处理 Ping/Pong
    if (opcode == 0x9) { // Ping
        send_frame(client, 0xA, *payload, *payload_len);
        free(*payload);
        return 0; // 非数据帧
    } else if (opcode == 0xA) { // Pong
        free(*payload);
        return 0; // 忽略 Pong
    }

    // 处理分片
    if (opcode == 0x1 || opcode == 0x2 || opcode == 0x0) { // 文本、二进制或延续帧
        *fragment_buf = (char *)realloc(*fragment_buf, *fragment_len + *payload_len);
        if (!*fragment_buf) {
            free(*payload);
            return -1;
        }
        memcpy(*fragment_buf + *fragment_len, *payload, *payload_len);
        *fragment_len += *payload_len;
        free(*payload);

        if (fin) {
            *payload = *fragment_buf;
            *payload_len = *fragment_len;
            *fragment_buf = NULL;
            *fragment_len = 0;
            return 1; // 完整消息
        }
        return 0; // 等待更多分片
    }

    free(*payload);
    return -1; // 未知帧
}

// 查找或添加客户端
Client *find_or_add_client(SOCKET socket) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].socket == socket) return &clients[i];
    }
    if (client_count < MAX_CLIENTS) {
        clients[client_count].socket = socket;
        clients[client_count].fragment_buf = NULL;
        clients[client_count].fragment_len = 0;
        clients[client_count].handshaked = 0;
        return &clients[client_count++];
    }
    return NULL;
}

// 移除客户端
void remove_client(SOCKET socket) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].socket == socket) {
            free(clients[i].fragment_buf);
            closesocket(clients[i].socket);
            clients[i] = clients[--client_count];
            break;
        }
    }
}

// Windows 消息处理
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_SOCKET) {
        SOCKET socket = (SOCKET)wParam;
        int event = WSAGETSELECTEVENT(lParam);
        int error = WSAGETSELECTERROR(lParam);

        if (error) {
            remove_client(socket);
            return 0;
        }

        Client *client = find_or_add_client(socket);
        if (!client) return 0;

        if (event == FD_ACCEPT) {
            struct sockaddr_in client_addr;
            int client_len = sizeof(client_addr);
            SOCKET new_socket = accept(socket, (struct sockaddr*)&client_addr, &client_len);
            if (new_socket != INVALID_SOCKET) {
                Client *new_client = find_or_add_client(new_socket);
                if (new_client) {
                    WSAAsyncSelect(new_socket, hwnd, WM_SOCKET, FD_READ | FD_CLOSE);
                } else {
                    closesocket(new_socket);
                }
            }
        } else if (event == FD_READ) {
            char *buffer = (char *)malloc(16384); // 初始缓冲区 16KB
            if (!buffer) {
                remove_client(socket);
                return 0;
            }
            int len = recv(socket, buffer, 16384, 0);
            if (len <= 0) {
                remove_client(socket);
                free(buffer);
                return 0;
            }

            if (!client->handshaked) {
                buffer[len] = '\0';
                if (strstr(buffer, "GET / HTTP/1.1") && strstr(buffer, "Upgrade: websocket")) {
                    char *key_start = strstr(buffer, "Sec-WebSocket-Key: ") + 19;
                    char key[64] = {0};
                    sscanf(key_start, "%s", key);
                    char accept[64];
                    compute_websocket_accept(key, accept);
                    char response[256];
                    snprintf(response, sizeof(response),
                        "HTTP/1.1 101 Switching Protocols\r\n"
                        "Upgrade: websocket\r\n"
                        "Connection: Upgrade\r\n"
                        "Sec-WebSocket-Accept: %s\r\n\r\n", accept);
                    send(socket, response, strlen(response), 0);
                    client->handshaked = 1;
                }
                free(buffer);
            } else {
                char *payload;
                size_t payload_len;
                int result = parse_frame(socket, buffer, len, &payload, &payload_len, &client->fragment_buf, &client->fragment_len);
                free(buffer);
                if (result == 1) { // 完整消息
                    printf("Received from %lld: %s\n", (long long)socket, payload);
                    send_frame(socket, 0x1, payload, payload_len);
                    free(payload);
                } else if (result == -2) { // 关闭帧
                    remove_client(socket);
                }
            }
        } else if (event == FD_CLOSE) {
            remove_client(socket);
        }
    } else if (msg == WM_TIMER && wParam == WM_PING_TIMER) {
        // 定时发送 Ping 帧
        char ping_data[] = "Ping";
        for (int i = 0; i < client_count; i++) {
            if (clients[i].handshaked) {
                send_frame(clients[i].socket, 0x9, ping_data, strlen(ping_data));
                printf("Sent Ping to %lld\n", (long long)clients[i].socket);
            }
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int main() {
    WSADATA wsaData;
    SOCKET server_socket;
    struct sockaddr_in server;

    // 初始化 Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // 创建窗口类
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WebSocketServer";
    RegisterClass(&wc);

    // 创建隐藏窗口
    HWND hwnd = CreateWindow(L"WebSocketServer", NULL, 0, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);

    // 设置定时器，每 30 秒触发
    SetTimer(hwnd, WM_PING_TIMER, PING_INTERVAL, NULL);

    // 创建套接字
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        KillTimer(hwnd, WM_PING_TIMER);
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);
    if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(server_socket);
        KillTimer(hwnd, WM_PING_TIMER);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(server_socket);
        KillTimer(hwnd, WM_PING_TIMER);
        WSACleanup();
        return 1;
    }

    // 设置异步事件
    if (WSAAsyncSelect(server_socket, hwnd, WM_SOCKET, FD_ACCEPT) == SOCKET_ERROR) {
        printf("WSAAsyncSelect failed: %d\n", WSAGetLastError());
        closesocket(server_socket);
        KillTimer(hwnd, WM_PING_TIMER);
        WSACleanup();
        return 1;
    }

    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 清理
    for (int i = 0; i < client_count; i++) {
        free(clients[i].fragment_buf);
        closesocket(clients[i].socket);
    }
    closesocket(server_socket);
    KillTimer(hwnd, WM_PING_TIMER);
    WSACleanup();
    DestroyWindow(hwnd);
    return 0;
}