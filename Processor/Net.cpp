#include <pch.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <netlistmgr.h>
#include <comdef.h>

#include "NetConnListener.h"


#include "Net.h"
namespace {
    std::unique_ptr<Net> net;
    static std::unordered_map<std::string, void (Net::*)(const rapidjson::Value&, JsonResult*)> funcs{
    {"getAddress", &Net::getAddress},
    };
    // todo: 创建一个命名管道，接收数据
    // todo: 获取网卡地址
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
bool Net::execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Net::get()->*it->second)(param, result);
    return true;
}
void Net::getAddress(const rapidjson::Value& params, JsonResult* result)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        result->addErr("WSAStartup failed");
        return;
    }

    // 设置初始缓冲区大小
    ULONG bufferSize = 15000;
    std::vector<char> buffer(bufferSize);
    PIP_ADAPTER_ADDRESSES adapters = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());

    // 获取适配器信息,支持 IPv4 和 IPv6
    DWORD dr = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr, adapters, &bufferSize);
    if (dr == ERROR_BUFFER_OVERFLOW) {
        buffer.resize(bufferSize);
        adapters = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());
        dr = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr, adapters, &bufferSize);
    }
    if (dr != ERROR_SUCCESS) {
        result->addErr("GetAdaptersAddresses failed");
        WSACleanup();
        return;
    }

    rapidjson::Value array(rapidjson::kArrayType);
    for (PIP_ADAPTER_ADDRESSES adapter = adapters; adapter; adapter = adapter->Next) {
        // 跳过未启用的适配器
        if (adapter->OperStatus != IfOperStatusUp) {
            continue;
        }

        // 创建适配器对象
        rapidjson::Value adapterObj(rapidjson::kObjectType);

        // 获取适配器名称
        std::wstring wideName = adapter->FriendlyName;
        std::string adapterName(wideName.begin(), wideName.end());
        rapidjson::Value nameVal;
        nameVal.SetString(adapterName.c_str(), adapterName.length(), result->getAllocator());
        adapterObj.AddMember("name", nameVal, result->getAllocator());

        // 获取物理地址（MAC地址）
        rapidjson::Value macVal;
        if (adapter->PhysicalAddressLength > 0) {
            char macStr[18]; // 格式为 XX-XX-XX-XX-XX-XX
            snprintf(macStr, sizeof(macStr), "%02X-%02X-%02X-%02X-%02X-%02X",
                adapter->PhysicalAddress[0], adapter->PhysicalAddress[1],
                adapter->PhysicalAddress[2], adapter->PhysicalAddress[3],
                adapter->PhysicalAddress[4], adapter->PhysicalAddress[5]);
            macVal.SetString(macStr, strlen(macStr), result->getAllocator());
        }
        else {
            continue;
        }
        adapterObj.AddMember("mac", macVal, result->getAllocator());

        // 获取IP地址
        rapidjson::Value ipArray(rapidjson::kArrayType);
        for (PIP_ADAPTER_UNICAST_ADDRESS unicast = adapter->FirstUnicastAddress; unicast; unicast = unicast->Next)
        {
            SOCKET_ADDRESS& sockAddr = unicast->Address;
            std::string addr;
            if (sockAddr.lpSockaddr->sa_family == AF_INET) { // IPv4
                char buffer[INET_ADDRSTRLEN];
                sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(sockAddr.lpSockaddr);
                addr = inet_ntop(AF_INET, &(ipv4->sin_addr), buffer, sizeof(buffer));
                if (addr == "127.0.0.1") {
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
                val.SetString(addr.c_str(), addr.length(), result->getAllocator());
                ipArray.PushBack(val, result->getAllocator());
            }
        }
        adapterObj.AddMember("addresses", ipArray, result->getAllocator());
        array.PushBack(adapterObj, result->getAllocator());
    }

    WSACleanup();
    result->addValue("data", std::move(array));
}
void Net::on(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::string eventName = arr[0].GetString();
    auto tar = result->getTar();
    //tar->events[eventName].insert(result->winId);
}

void Net::off(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::string eventName = arr[0].GetString();
    auto tar = result->getTar();
    //tar->events[eventName].erase(result->winId);
}

void Net::regNetConnListener()
{
    INetworkListManagerPtr spNLM;
    auto hr = spNLM.CreateInstance(__uuidof(NetworkListManager));
    if (FAILED(hr)) {
        std::cerr << "Failed to create NetworkListManager: " << std::hex << hr << std::endl;
        return;
    }

    // 创建事件接收对象
    NetConnListener* pSink = new NetConnListener();
    if (!pSink) {
        std::cerr << "Failed to allocate sink" << std::endl;
        return;
    }

    // 获取连接点容器
    IConnectionPointContainer* pCPC = nullptr;
    hr = spNLM->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
    if (FAILED(hr)) {
        std::cerr << "QueryInterface for IConnectionPointContainer failed: " << std::hex << hr << std::endl;
        pSink->Release();
        return;
    }

    // 查找 INetworkListManagerEvents 的连接点
    IConnectionPoint* pCP = nullptr;
    hr = pCPC->FindConnectionPoint(__uuidof(INetworkListManagerEvents), &pCP);
    if (FAILED(hr)) {
        std::cerr << "FindConnectionPoint failed: " << std::hex << hr << std::endl;
        pCPC->Release();
        pSink->Release();
        return;
    }

    // 注册事件接收器
    DWORD dwCookie = 0;
    hr = pCP->Advise(pSink, &dwCookie);
    if (FAILED(hr)) {
        std::cerr << "Advise failed: " << std::hex << hr << std::endl;
        pCP->Release();
        pCPC->Release();
        pSink->Release();
        return;
    }
}
