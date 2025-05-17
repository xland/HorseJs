#pragma once
#include <windows.h>
#include <wrl.h>
#include <wil/com.h>
#include <WebView2.h>
#include <string>
#include <vector>

using namespace Microsoft::WRL;

// 简单的 COM 对象接口
struct __declspec(uuid("12345678-1234-1234-1234-1234567890AB")) IHostObject : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetMessage(BSTR* message) = 0;
};

class HostObject : public Implements<IHostObject> {
public:
    HRESULT STDMETHODCALLTYPE GetMessage(BSTR* message) override {
        *message = SysAllocString(L"Hello from C++ COM Object!");
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override 
    {
        if (riid == IID_IUnknown || riid == __uuidof(IHostObject)) {
            *ppvObject = static_cast<IHostObject*>(this);
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override 
    {
        return InterlockedIncrement(&refCount_);
    }

    ULONG STDMETHODCALLTYPE Release() override 
    {
        ULONG newRefCount = InterlockedDecrement(&refCount_);
        if (newRefCount == 0) {
            delete this;
        }
        return newRefCount;
    }

private:
    ULONG refCount_ = 1;
};
