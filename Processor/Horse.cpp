#include <pch.h>
#include "Horse.h"
#include "../App/App.h"
#include "../App/BrowserWindow.h"
#include "../Res/Res.h"

namespace {
    std::unique_ptr<Horse> horse;
    static std::unordered_map<std::string, void (Horse::*)(const rapidjson::Value&, JsonResult*)> funcs{
    {"getConfig", &Horse::getConfig},
    {"createWin", &Horse::createWin},
    {"getExeVer", &Horse::getExeVer},
    {"getHorseVer", &Horse::getHorseVer},
    {"quit", &Horse::quit},
    {"exit", &Horse::exit},
    {"relaunch", &Horse::relaunch},
    {"enableSecondIns", &Horse::enableSecondIns},
    {"disableSecondIns", &Horse::disableSecondIns},
    {"packRes", &Horse::packRes},
    {"saveRes", &Horse::saveRes},
    {"autoStart", &Horse::autoStart},
    {"on", &Horse::on},
    {"off", &Horse::off},
    };

    //todo  Application User Model IDs  https://learn.microsoft.com/en-us/windows/win32/shell/appids
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

void Horse::createWin(const rapidjson::Value& params, JsonResult* result)
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
void Horse::enableSecondIns(const rapidjson::Value& params, JsonResult* result)
{
    auto app = App::get();
    CloseHandle(App::get()->singleInsMutext);
    app->instanceLock = false;
}
void Horse::disableSecondIns(const rapidjson::Value& params, JsonResult* result)
{
    auto app = App::get();
    auto mutextName = "Global\\" + app->appId;
    auto nameStr = Util::convertToWStr(mutextName.data());
    auto singleInsMutext = CreateMutex(NULL, TRUE, nameStr.data());
    if (GetLastError()) {
        result->addErr("already disable");
        return;
    }
    app->singleInsMutext = singleInsMutext;
    app->instanceLock = true;
}

void Horse::packRes(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring src = Util::convertToWStr(arr[0].GetString());
    std::filesystem::path targetPath(src);

    HANDLE handle = BeginUpdateResource(src.c_str(), FALSE);
    if (!handle) {
        result->addErr("BeginUpdateResource failed");
        return;
    }
    auto pPath = targetPath.parent_path() / "UI";
    auto pPathStr = pPath.wstring();
    std::vector<std::wstring> fileList;
    enumFiles(pPathStr, pPathStr, fileList);
    for (auto& fileStr : fileList)
    {
        auto flag = addResToExe(handle, fileStr, pPathStr);
        if (!flag) {
            result->addErr("add file to exe error: " + Util::convertToStr(fileStr));
            break;
        }
    }
    if (!EndUpdateResource(handle, FALSE)) {
        result->addErr("EndUpdateResource failed");
    }
}

void Horse::saveRes(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring resName = Util::convertToWStr(arr[0].GetString());
    std::wstring tarPath = Util::convertToWStr(arr[1].GetString());
    bool inDataDir = arr[2].GetBool();
    if (inDataDir) {
        auto p = App::get()->appDir / tarPath;
        tarPath = p.make_preferred().wstring();
    }
    std::vector<std::filesystem::path> directories;
    std::filesystem::path current(tarPath);
    current = current.parent_path();
    while (!current.empty() && !std::filesystem::exists(current)) {
        directories.push_back(current);
        current = current.parent_path();
    }
    for (auto it = directories.rbegin(); it != directories.rend(); ++it) {
        if (!CreateDirectory(it->wstring().c_str(), nullptr)) {
            DWORD error = GetLastError();
            if (error != ERROR_ALREADY_EXISTS) {
                result->addErr("create path exists");
                return;
            }
        }
    }
    HRSRC hRes = FindResource(NULL, resName.c_str(), RT_RCDATA);
    if (!hRes) {
        result->addErr("no resName");
        return;
    }
    HGLOBAL hData = LoadResource(NULL, hRes);
    if (!hData) {
        result->addErr("load resName err");
        return;
    }
    void* pData = LockResource(hData);
    DWORD size = SizeofResource(NULL, hRes);
    std::ofstream outFile(tarPath, std::ios::binary);
    outFile.write(reinterpret_cast<const char*>(pData), size);

}
void Horse::autoStart(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto flag = arr[0].GetBool();
    HKEY hKey;
    std::wstring runKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    std::wstring valueName = Util::convertToWStr(App::get()->appId.data());
    if (flag) {
        wchar_t buffer[MAX_PATH];
        GetModuleFileName(nullptr, buffer, MAX_PATH);
        auto curPath = std::filesystem::path(buffer);
        std::wstring exePath = curPath.wstring();
        if (RegOpenKeyEx(HKEY_CURRENT_USER, runKey.data(), 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
            RegSetValueEx(hKey, valueName.data(), 0, REG_SZ, (const BYTE*)exePath.data(), exePath.size() * sizeof(wchar_t));
            RegCloseKey(hKey);
            return;
        }
    }
    else {
        if (RegOpenKeyEx(HKEY_CURRENT_USER, runKey.data(), 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
            RegDeleteValue(hKey, valueName.data());
            RegCloseKey(hKey);
            return;
        }
    }
    result->addErr("edit reg err");

}
void Horse::on(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::string eventName = arr[0].GetString();
    App::get()->events[eventName].insert(result->winId);
}
void Horse::off(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::string eventName = arr[0].GetString();
    App::get()->events[eventName].erase(result->winId);
}
