#include "App.h"
#include "../Win/WinBase.h"
namespace {
    std::shared_ptr<App> app;
}
using namespace Microsoft::WRL;


App::App()
{
}

App::~App()
{
}

App* App::get()
{
    return app.get();
}

void App::init()
{
    app = std::make_shared<App>();
    app->start();
}

void App::start()
{
    if (!checkRuntime()) {
        return;
    }
    auto envReadyInstance = Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(this, &App::envReady);
    HRESULT hr = CreateCoreWebView2Environment(envReadyInstance.Get()); 
    //todo CreateCoreWebView2EnvironmentWithOptions    
    //WebView2 Warning: Using default User Data Folder is not recommended, please see documentation.  https://go.microsoft.com/fwlink/?linkid=2187341 
    if (FAILED(hr)) {
        MessageBox(nullptr, L"创建 WebView2 环境失败", L"错误", MB_OK | MB_ICONERROR);
        // 根据需要处理错误，例如退出应用程序
        return;
    }
}




bool App::checkRegKey(const HKEY& key, const std::wstring& subKey) {
    size_t bufferSize = 20;
    std::wstring valueBuf;
    valueBuf.resize(bufferSize);
    auto valueSize = static_cast<DWORD>(bufferSize * sizeof(wchar_t));
    auto rc = RegGetValue(key, subKey.c_str(), L"pv", RRF_RT_REG_SZ, nullptr, static_cast<void*>(valueBuf.data()), &valueSize);
    if (rc == ERROR_SUCCESS)
    {
        valueSize /= sizeof(wchar_t);
        valueBuf.resize(static_cast<size_t>(valueSize - 1));
        if (valueBuf.empty() || valueBuf == L"0.0.0.0") {
            return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}

HRESULT App::envReady(HRESULT result, ICoreWebView2Environment* env)
{
    this->env = env;
    auto content = Util::readFile(L"config.json");
    rapidjson::Document d;
    d.Parse(content.data());
    auto win = WinBase::create(d["window"]);
    return S_OK;
}


bool App::checkRuntime()
{
    std::wstring regSubKey = L"\\Microsoft\\EdgeUpdate\\Clients\\{F3017226-FE2A-4295-8BDF-00C3A9A7E4C5}";
    bool hasRuntime = checkRegKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node" + regSubKey);
    if (hasRuntime) return true;
    hasRuntime = checkRegKey(HKEY_CURRENT_USER, L"Software" + regSubKey);
    if (!hasRuntime) {
        auto result = MessageBox(nullptr, L"缺少WebView2系统组件",L"系统提示", MB_OKCANCEL | MB_ICONINFORMATION | MB_DEFBUTTON1);
        if (result == IDOK) {
            ShellExecute(0, 0, L"https://go.microsoft.com/fwlink/p/?LinkId=2124703", 0, 0, SW_SHOW);
        }
        return false;
    }
    return true;
}
