#pragma once
#include <netlistmgr.h>
#include <comdef.h>

_COM_SMARTPTR_TYPEDEF(INetworkListManager, __uuidof(INetworkListManager));
_COM_SMARTPTR_TYPEDEF(INetworkListManagerEvents, __uuidof(INetworkListManagerEvents));

class NetConnListener : public INetworkListManagerEvents
{
public:
	NetConnListener();
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	HRESULT STDMETHODCALLTYPE ConnectivityChanged(NLM_CONNECTIVITY newConnectivity);
private:
	LONG m_cRef;
};

