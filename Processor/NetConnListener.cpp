#include "pch.h"
#include "NetConnListener.h"


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
    if (newConnectivity == NLM_CONNECTIVITY_DISCONNECTED) {
        std::cout << "Network Disconnected!" << std::endl;
    }
    else {
        std::cout << "Network Connected! Connectivity Flags: ";
        if (newConnectivity & NLM_CONNECTIVITY_IPV4_INTERNET) {
            std::cout << "IPv4 Internet ";
        }
        if (newConnectivity & NLM_CONNECTIVITY_IPV6_INTERNET) {
            std::cout << "IPv6 Internet ";
        }
        if (newConnectivity & NLM_CONNECTIVITY_IPV4_LOCALNETWORK) {
            std::cout << "IPv4 Local Network ";
        }
        if (newConnectivity & NLM_CONNECTIVITY_IPV6_LOCALNETWORK) {
            std::cout << "IPv6 Local Network ";
        }
        std::cout << std::endl;
    }
    return S_OK;
}