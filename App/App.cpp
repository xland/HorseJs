#include <pch.h>

#include "App.h"
#include "../App/BrowserWindow.h"
using namespace Microsoft;
namespace {
    std::shared_mutex mtx;
    std::unique_ptr<App> app;
    static std::vector<std::unique_ptr<BrowserWindow>> wins;
}


App::App(HINSTANCE hInstance) :hInstance{hInstance}
{
}

App::~App()
{
}

void App::onWindowDestroy(BrowserWindow* win)
{
    std::unique_lock<std::shared_mutex> lock(mtx);
    std::erase_if(wins, [win](const std::unique_ptr<BrowserWindow>& ptr) { return ptr->id == win->id; });
    if (quitWhenAllWindowClosed && wins.empty()) {
        PostQuitMessage(0);
    }    
}

App* App::get()
{
    return app.get();
}

BrowserWindow* App::getWindow(const int& id)
{
    std::shared_lock<std::shared_mutex> lock(mtx);
    auto it = std::find_if(wins.begin(),wins.end(), [&id](const std::unique_ptr<BrowserWindow>& ptr) { return ptr->id == id; });
    if (it != wins.end()) {
        return (*it).get();
    }
}

void App::addWindow(std::unique_ptr<BrowserWindow> win)
{
    std::unique_lock<std::shared_mutex> lock(mtx);
    wins.push_back(std::move(win));
}

void App::init(HINSTANCE hInstance)
{
    app = std::make_unique<App>(hInstance);
    app->start();
}
void App::start()
{
    checkRuntime();
    ensureAppFolder();
    loadConfig();
    createEnv();
}
void App::checkRuntime()
{
    // --disable-features=msSmartScreenProtection
    std::wstring regSubKey = L"\\Microsoft\\EdgeUpdate\\Clients\\{F3017226-FE2A-4295-8BDF-00C3A9A7E4C5}";
    bool hasRuntime = checkRegKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node" + regSubKey);
    if (hasRuntime) return;
    hasRuntime = checkRegKey(HKEY_CURRENT_USER, L"Software" + regSubKey);
    if (!hasRuntime) {
        auto result = MessageBox(nullptr, L"缺少WebView2系统组件",L"系统提示", MB_OKCANCEL | MB_ICONINFORMATION | MB_DEFBUTTON1);
        if (result == IDOK) {
            ShellExecute(0, 0, L"https://go.microsoft.com/fwlink/p/?LinkId=2124703", 0, 0, SW_SHOW);
        }
        exit(2);
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
void App::ensureAppFolder() {

    PWSTR pathTmp;
    auto ret = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &pathTmp);
    if (ret != S_OK) {
        CoTaskMemFree(pathTmp);
        auto result = MessageBox(nullptr, L"无法得到系统数据目录地址", L"系统提示", MB_OK | MB_ICONINFORMATION | MB_DEFBUTTON1);
        exit(1);
    }
    appDir = pathTmp;
    CoTaskMemFree(pathTmp);
    appDir /= appId;
    if (!std::filesystem::exists(appDir)) {
        auto flag = std::filesystem::create_directory(appDir);
        if (!flag) {
            MessageBox(nullptr, L"无法创建应用程序数据目录", L"系统提示", MB_OK | MB_ICONINFORMATION | MB_DEFBUTTON1);
            exit(1);
        }
    }
}
void App::loadConfig()
{
    auto jsonData = Util::readFile(L"UI/config.json");
    rapidjson::Document jsonDoc;
    jsonDoc.Parse(jsonData.data());
    if (jsonDoc.HasMember("appId") && jsonDoc["appId"].IsString())
    {
        appId = jsonDoc["appId"].GetString();
    }
    if (jsonDoc.HasMember("quitWhenAllWindowClosed") && jsonDoc["quitWhenAllWindowClosed"].IsBool())
    {
        quitWhenAllWindowClosed = jsonDoc["quitWhenAllWindowClosed"].GetBool();
    }
    auto win = std::make_unique<BrowserWindow>(jsonDoc["window"]);
    wins.push_back(std::move(win));
}
void App::createEnv()
{
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
    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(nullptr, appDir.c_str(), nullptr, envReadyInstance.Get());
    if (FAILED(hr)) {
        MessageBox(nullptr, L"创建 WebView2 环境失败", L"错误", MB_OK | MB_ICONERROR);
        return;
    }
}
HRESULT App::envReady(HRESULT result, ICoreWebView2Environment* env)
{
    this->env = env;
    wins[0]->load();
    return S_OK;
}