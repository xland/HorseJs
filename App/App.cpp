#include <Shlobj.h>
#include <shellapi.h>
#include <fstream>
#include <filesystem>
#include <functional>
#include <wrl.h>
#include <wil/com.h>
#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>

#include "App.h"
#include "AppConfig.h"
#include "Fs.h"
#include "../Win/BrowserWindow.h"
using namespace Microsoft;
namespace {
    std::unique_ptr<App> app;
}


App::App() :config{std::make_unique<AppConfig>()}
{
}

App::~App()
{
}

void App::onWindowDestroy(BrowserWindow* win)
{
    windows.erase(std::remove_if(windows.begin(), windows.end(),
        [win](const std::unique_ptr<BrowserWindow>& ptr) {
            return ptr.get() == win;
        }),
        windows.end());
    if (config->quitWhenAllWindowClosed && windows.empty()) {
        PostQuitMessage(0);
    }
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

    if (!checkRuntime()) {
        return;
    }
    auto path = ensureAppFolder();
    if (path.empty()) {
        return;
    }
    Fs::init();

    //auto options = WRL::Make<CoreWebView2EnvironmentOptions>();
    //options->put_AdditionalBrowserArguments(L"--allow-file-access-from-files");
    //WRL::ComPtr<ICoreWebView2EnvironmentOptions4> options4;
    //HRESULT oeResult = options.As(&options4);
    //if (oeResult != S_OK) {
    //    // UNREACHABLE - cannot continue  todo
    //}
    //const WCHAR* allowedSchemeOrigins[5] = { L"about://*", L"http://*", L"https://*", L"file://*", L"socket://*" };
    //auto defaultRegistration = WRL::Make<CoreWebView2CustomSchemeRegistration>(L"horse");
    //defaultRegistration->put_HasAuthorityComponent(TRUE);
    //defaultRegistration->put_TreatAsSecure(TRUE);
    //defaultRegistration->SetAllowedOrigins(5, allowedSchemeOrigins);
    //ICoreWebView2CustomSchemeRegistration* registrations[1] = { defaultRegistration.Get() };
    //options4->SetCustomSchemeRegistrations(1, static_cast<ICoreWebView2CustomSchemeRegistration**>(registrations));
    //HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(nullptr, path.c_str(), options.Get(),envReadyInstance.Get());



    auto envReadyInstance = WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(this, &App::envReady);
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
    path /= config->appId;
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



    //wil::com_ptr<ICoreWebView2ControllerOptions> controllerOptions;
    //env4->CreateCoreWebView2CompositionController(&controllerOptions);


    auto& winConfig = config->getFirstWindowConfig();
    auto win = std::make_unique<BrowserWindow>(winConfig);
    win->load(winConfig["page"]);
    config->releaseJsonDoc();
    windows.push_back(std::move(win));   
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
