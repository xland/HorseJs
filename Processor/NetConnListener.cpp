#include "pch.h"
#include "JsonResult.h"
#include "NetConnListener.h"

namespace {
    IConnectionPoint* pCP{ nullptr };
    NetConnListener* pSink{ nullptr };
    IConnectionPointContainer* pCPC{ nullptr };
    DWORD dwCookie = 0;
    std::set<int> eventWinIds;
}

NetConnListener::NetConnListener() : m_cRef(1)
{
}
HRESULT STDMETHODCALLTYPE NetConnListener::QueryInterface(REFIID riid, void** ppv) {
    if (!ppv) return E_INVALIDARG;
    *ppv = nullptr;

    if (riid == IID_IUnknown) {
        *ppv = static_cast<IUnknown*>(this);
    }
    else if (riid == __uuidof(INetworkListManagerEvents)) {
        *ppv = static_cast<INetworkListManagerEvents*>(this);
    }
    else {
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}
ULONG STDMETHODCALLTYPE NetConnListener::AddRef() {
    return InterlockedIncrement(&m_cRef);
}
ULONG STDMETHODCALLTYPE NetConnListener::Release() {
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0) {
        delete this;
        return 0;
    }
    return cRef;
}
// INetworkListManagerEvents 接口实现
HRESULT STDMETHODCALLTYPE NetConnListener::ConnectivityChanged(NLM_CONNECTIVITY newConnectivity) {
    for (auto& winId:eventWinIds)
    {
        if (newConnectivity == NLM_CONNECTIVITY_DISCONNECTED) {
            JsonResult result(winId, "net", "connChanged");
            result.addString("data", "disconn");
            result.returnBack();
        }
        else {
            if (newConnectivity & NLM_CONNECTIVITY_IPV4_INTERNET) {
                JsonResult result(winId, "net", "connChanged");
                result.addString("data", "conn");
                result.addString("type", "ipv4");
                result.returnBack();
            }else if (newConnectivity & NLM_CONNECTIVITY_IPV6_INTERNET) {
                JsonResult result(winId, "net", "connChanged");
                result.addString("data", "conn");
                result.addString("type", "ipv6");
                result.returnBack();
            }else if (newConnectivity & NLM_CONNECTIVITY_IPV4_LOCALNETWORK) {
                JsonResult result(winId, "net", "connChanged");
                result.addString("data", "conn");
                result.addString("type", "ipv4Local");
                result.returnBack();
            }else if (newConnectivity & NLM_CONNECTIVITY_IPV6_LOCALNETWORK) {
                JsonResult result(winId, "net", "connChanged");
                result.addString("data", "conn");
                result.addString("type", "ipv6Local");
                result.returnBack();
            }
        }
    }
    return S_OK;
}

bool NetConnListener::listen(const int& winId)
{
    eventWinIds.insert(winId);
    if (pSink) {
        return true;
    }
    INetworkListManagerPtr spNLM;
    auto hr = spNLM.CreateInstance(__uuidof(NetworkListManager));
    if (FAILED(hr)) {
        std::cerr << "Failed to create NetworkListManager: " << std::hex << hr << std::endl;
        return false;
    }

    // 创建事件接收对象
    pSink = new NetConnListener();
    if (!pSink) {
        std::cerr << "Failed to allocate sink" << std::endl;
        return false;
    }

    // 获取连接点容器
    
    hr = spNLM->QueryInterface(IID_IConnectionPointContainer, (void**)&pCPC);
    if (FAILED(hr)) {
        std::cerr << "QueryInterface for IConnectionPointContainer failed: " << std::hex << hr << std::endl;
        pSink->Release();
        return false;
    }

    // 查找 INetworkListManagerEvents 的连接点
    
    hr = pCPC->FindConnectionPoint(__uuidof(INetworkListManagerEvents), &pCP);
    if (FAILED(hr)) {
        std::cerr << "FindConnectionPoint failed: " << std::hex << hr << std::endl;
        pCPC->Release();
        pSink->Release();
        return false;
    }

    // 注册事件接收器
    
    hr = pCP->Advise(pSink, &dwCookie);
    if (FAILED(hr)) {
        std::cerr << "Advise failed: " << std::hex << hr << std::endl;
        pCP->Release();
        pCPC->Release();
        pSink->Release();
        return false;
    }
    return true;
}

bool NetConnListener::unlisten(const int& winId)
{
    eventWinIds.erase(winId);
    if (!eventWinIds.empty()) return true;
    pCP->Unadvise(dwCookie);
    pCP->Release();
    pCPC->Release();
    pSink->Release();
    pCP = nullptr;
    pCPC = nullptr;
    pSink = nullptr;
    dwCookie = 0;
    return true;
}
