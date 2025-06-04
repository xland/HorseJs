#include <pch.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>


#include "Net.h"

namespace {
    std::unique_ptr<Net> net;
}

Net::Net()
{
}

Net::~Net()
{
}

Net* Net::get()
{
    if(!net) {
        net = std::make_unique<Net>();
	}
    return net.get();
}

void Net::getAddress(const rapidjson::Value& params, JsonResult* result)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
    }
    // 设置初始缓冲区大小
    ULONG bufferSize = 15000; // 初始估计
    std::vector<char> buffer(bufferSize);
    PIP_ADAPTER_ADDRESSES adapters = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());
    // 获取适配器信息,支持 IPv4 和 IPv6
    DWORD dr = GetAdaptersAddresses(AF_UNSPEC,GAA_FLAG_INCLUDE_PREFIX,nullptr,adapters,&bufferSize);
    // 如果缓冲区不足，重新分配
    if (dr == ERROR_BUFFER_OVERFLOW) {
        buffer.resize(bufferSize);
        adapters = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());
        dr = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr, adapters, &bufferSize);
    }
    if (dr != ERROR_SUCCESS) {
        std::cerr << "GetAdaptersAddresses failed: " << dr << std::endl;
        WSACleanup();
    }
    rapidjson::Value array(rapidjson::kArrayType);
    // 遍历适配器
    for (PIP_ADAPTER_ADDRESSES adapter = adapters; adapter; adapter = adapter->Next) {
        // 跳过未启用的适配器
        if (adapter->OperStatus != IfOperStatusUp) {
            continue;
        }
        // 遍历单播地址
        for (PIP_ADAPTER_UNICAST_ADDRESS unicast = adapter->FirstUnicastAddress; unicast; unicast = unicast->Next) 
        {
            SOCKET_ADDRESS& sockAddr = unicast->Address;
            std::string addr;
            if (sockAddr.lpSockaddr->sa_family == AF_INET) { // IPv4
                char buffer[INET_ADDRSTRLEN];
                sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(sockAddr.lpSockaddr);
                addr = inet_ntop(AF_INET, &(ipv4->sin_addr), buffer, sizeof(buffer));
                if (addr == "127.0.0.1") { // 跳过回环地址
                    continue;
                }
            }
            else if (sockAddr.lpSockaddr->sa_family == AF_INET6) { // IPv6
                char buffer[INET6_ADDRSTRLEN];
                sockaddr_in6* ipv6 = reinterpret_cast<sockaddr_in6*>(sockAddr.lpSockaddr);
                addr = inet_ntop(AF_INET6, &(ipv6->sin6_addr), buffer, sizeof(buffer));
                if (addr == "::1") {
                    continue;
                }
            }
            if (!addr.empty()) {
                rapidjson::Value val;
                val.SetString(addr.data(), addr.length(), result->getAllocator());
                array.PushBack(val, result->getAllocator());
            }
        }
    }
    WSACleanup();
    result->addValue("data", std::move(array));
    result->returnBack();
}
