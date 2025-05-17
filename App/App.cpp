#include "App.h"
#include "../Win/WinBase.h"
namespace {
    std::unique_ptr<App> app;
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
    app = std::make_unique<App>();
    app->start();
}

void App::start()
{
    auto content = Util::readFile(L"config.json");
    d.Parse(content.data());
    if (!checkRuntime()) {
        return;
    }
    auto path = ensureAppFolder();
    if (path.empty()) {
        return;
    }
    auto envReadyInstance = Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(this, &App::envReady);
    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(nullptr, path.c_str(), nullptr,envReadyInstance.Get());
    if (FAILED(hr)) {
        MessageBox(nullptr, L"创建 WebView2 环境失败", L"错误", MB_OK | MB_ICONERROR);
        return;
    }
}

std::filesystem::path App::ensureAppFolder() {
    std::filesystem::path path;
    PWSTR pathTmp;
    auto ret = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &pathTmp);
    if (ret != S_OK) {
        CoTaskMemFree(pathTmp);
        auto result = MessageBox(nullptr, L"无法得到系统数据目录地址", L"系统提示", MB_OK | MB_ICONINFORMATION | MB_DEFBUTTON1);
        exit(1);
        return path;
    }
    path = pathTmp;
    CoTaskMemFree(pathTmp);
    path /= d["appId"].GetString();
    if (!std::filesystem::exists(path)) {
        auto flag = std::filesystem::create_directory(path);
        if (!flag) {
            MessageBox(nullptr, L"无法创建应用程序数据目录", L"系统提示", MB_OK | MB_ICONINFORMATION | MB_DEFBUTTON1);
            exit(1);
        }
    }
    return path;
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
    auto win = WinBase::create(d["window"]); //todo
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
