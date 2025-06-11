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
    {"getVersion", &Horse::getVersion},
    {"quit", &Horse::quit},
    {"exit", &Horse::exit},
    {"relaunch", &Horse::relaunch},
    {"getPath", &Horse::getPath},
    };
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

void Horse::getVersion(const rapidjson::Value& params, JsonResult* result) 
{
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
    DWORD major = (fileInfo->dwFileVersionMS >> 16) & 0xFFFF;
    DWORD minor = fileInfo->dwFileVersionMS & 0xFFFF;
    DWORD patch = (fileInfo->dwFileVersionLS >> 16) & 0xFFFF;
    DWORD build = fileInfo->dwFileVersionLS & 0xFFFF;
    auto& allocator = result->getAllocator();
    rapidjson::Value array1(rapidjson::kArrayType);
    array1.PushBack(VERSION_MAJOR, allocator);
    array1.PushBack(VERSION_MINOR, allocator);
    array1.PushBack(VERSION_PATCH, allocator);
    array1.PushBack(VERSION_BUILD, allocator);
    result->addValue("exeVer", std::move(array1));
    rapidjson::Value array2(rapidjson::kArrayType);
    array2.PushBack(VERSION_MAJOR, allocator);
    array2.PushBack(VERSION_MINOR, allocator);
    array2.PushBack(VERSION_PATCH, allocator);
    array2.PushBack(VERSION_BUILD, allocator);
    result->addValue("horseVer", std::move(array2));
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

void Horse::getPath(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    if (arr.Size()< 1 || arr[0].IsNull() || !arr[0].IsString()) {
        result->addErr("type err");
        return;
    }
    std::string type{ arr[0].GetString() };
    if (type == "exeDir" || type == "exePath") {
        result->addString("data", getExePath(type));
    }
    else if (type == "download") {
        getKnownPath(FOLDERID_Downloads,result);
    }
    else if (type == "music") {
        getKnownPath(FOLDERID_Music, result);
    }
    else if (type == "video") {
        getKnownPath(FOLDERID_Videos, result);
    }
    else if (type == "picture") {
        getKnownPath(FOLDERID_Pictures, result);
    }
    else if (type == "document") {
        getKnownPath(FOLDERID_Documents, result);
    }
    else if (type == "startup") {
        getKnownPath(FOLDERID_Startup, result);
    }
    else if (type == "desktop") {
        getKnownPath(FOLDERID_Desktop, result);
    }
    else if (type == "font") {
        getKnownPath(FOLDERID_Fonts, result);
    }
    else if (type == "program") {
        getKnownPath(FOLDERID_ProgramFiles, result);
    }
    else if (type == "system") {
        getKnownPath(FOLDERID_System, result);
    }
    else if (type == "windows") {
        getKnownPath(FOLDERID_Windows, result);
    }
    else if (type == "profile") {
        getKnownPath(FOLDERID_Profile, result);
    }
    else if (type == "appdata") {
        getKnownPath(FOLDERID_LocalAppData, result);
    }
    else if (type == "roaming") {
        getKnownPath(FOLDERID_RoamingAppData, result);
    }
    else if (type == "cache") {
        getKnownPath(FOLDERID_InternetCache, result);
    }
    else if (type == "userdata") {
        auto str = App::get()->appDir.string();
        result->addString("data", str);
    }
}

std::string Horse::getExePath(const std::string& type)
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileName(nullptr, buffer, MAX_PATH);
    auto curPath = std::filesystem::path(buffer);
    if (type == "exeDir") {
        curPath = curPath.parent_path();
    }
    auto curPathStr = curPath.string();
    return curPathStr;
}

void Horse::getKnownPath(const GUID& type, JsonResult* result)
{
    PWSTR path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(type, 0, nullptr, &path);
    if (SUCCEEDED(hr)) {
        std::wstring strW(path);
        auto str = Util::convertToStr(strW);
        CoTaskMemFree(path);
        result->addString("data", str);
    }
    else {
        result->addErr("get known folder err");
    }
}
