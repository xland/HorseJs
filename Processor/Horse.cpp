#include <pch.h>
#include "Horse.h"
#include "../App/App.h"
#include "../App/BrowserWindow.h"
#include "../Res/Res.h"

namespace {
    std::unique_ptr<Horse> horse;
    static std::unordered_map<std::string, void (Horse::*)(const rapidjson::Value&, JsonResult*)> funcs{
    {"getConfig", &Horse::getConfig},
    {"createWindow", &Horse::createWindow},
    {"getExeVer", &Horse::getExeVer},
    {"getHorseVer", &Horse::getHorseVer},
    {"quit", &Horse::quit},
    {"exit", &Horse::exit},
    {"relaunch", &Horse::relaunch},
    {"on", &Horse::on},
    {"off", &Horse::off},
    };

    //todo 监听窗口创建事件
    //todo  Application User Model IDs  https://learn.microsoft.com/en-us/windows/win32/shell/appids
    //todo 进程锁，单例进程
    //todo  Show the platform's native emoji picker.
    //todo setProxy
}

Horse::Horse()
{
}

Horse::~Horse()
{
}

Horse* Horse::get()
{
    if(!horse) {
        horse = std::make_unique<Horse>();
	}
    return horse.get();
}
bool Horse::execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Horse::get()->*it->second)(param, result);
    return true;
}
void Horse::getConfig(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto str = Util::readFile(L"UI/config.json");
    rapidjson::Document doc;
    doc.Parse(str.data());
    rapidjson::Value copiedValue(doc, result->getAllocator());
	result->addValue("data", std::move(copiedValue));
}

void Horse::createWindow(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    const rapidjson::Value& value = arr[0];
    auto winIns = std::make_unique<BrowserWindow>(value);
    winIns->load();
    result->addNumber("id", winIns->id);
    App::addWindow(std::move(winIns));
}
void Horse::getExeVer(const rapidjson::Value& params, JsonResult* result) {
    std::vector<wchar_t> exePath(MAX_PATH);
    if (GetModuleFileName(nullptr, exePath.data(), static_cast<DWORD>(exePath.size())) == 0) {
        result->addErr("Failed to get executable path");
    }
    // 获取版本信息大小
    DWORD dummy;
    DWORD versionSize = GetFileVersionInfoSize(exePath.data(), &dummy);
    if (versionSize == 0) {
        result->addErr("Failed to get version info size");
    }
    // 分配缓冲区并获取版本信息
    std::vector<BYTE> versionData(versionSize);
    if (!GetFileVersionInfoW(exePath.data(), 0, versionSize, versionData.data())) {
        result->addErr("Failed to get version info");
    }
    // 查询固定版本信息（VS_FIXEDFILEINFO）
    VS_FIXEDFILEINFO* fileInfo = nullptr;
    UINT fileInfoSize = 0;
    if (!VerQueryValueW(versionData.data(), L"\\", reinterpret_cast<void**>(&fileInfo), &fileInfoSize)) {
        result->addErr("Failed to query version info");
    }
    // 提取版本号
    int major = (fileInfo->dwFileVersionMS >> 16) & 0xFFFF;
    int minor = fileInfo->dwFileVersionMS & 0xFFFF;
    int patch = (fileInfo->dwFileVersionLS >> 16) & 0xFFFF;
    int build = fileInfo->dwFileVersionLS & 0xFFFF;

    result->addNumber("major", major);
    result->addNumber("minor", minor);
    result->addNumber("patch", patch);
    result->addNumber("build", build);
}
void Horse::getHorseVer(const rapidjson::Value& params, JsonResult* result) 
{    
    result->addNumber("major", VERSION_MAJOR);
    result->addNumber("minor", VERSION_MINOR);
    result->addNumber("patch", VERSION_PATCH);
    result->addNumber("build", VERSION_BUILD);    
}

void Horse::quit(const rapidjson::Value& params, JsonResult* result)
{
    App::closeAllWindowAsync();
    for (size_t i = 0; i < 8; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (!App::hasWindow()) {
            PostQuitMessage(0);
            break;
        }
    }
    result->addErr("can not close all win");
}

void Horse::exit(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    int val = 0;
    if (!arr[0].IsNull() && arr[0].IsInt()) {
        auto val = arr[0].GetInt();
    }
    PostQuitMessage(val);
}

void Horse::relaunch(const rapidjson::Value& params, JsonResult* result)
{
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (CreateProcess(szPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}
void Horse::on(const rapidjson::Value& params, JsonResult* result)
{
}
void Horse::off(const rapidjson::Value& params, JsonResult* result)
{
}
