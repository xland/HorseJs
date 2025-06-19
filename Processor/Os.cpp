#include <pch.h>
#include "Os.h"

namespace {
    std::unique_ptr<Os> os;
    static std::unordered_map<std::string, void (Os::*)(const rapidjson::Value&, JsonResult*)> funcs{
    {"getVersion", &Os::getVersion},
    {"createShortcut", &Os::createShortcut},
    {"getCPUID", &Os::getCPUID},
    {"getDiskSerialNumber", &Os::getDiskSerialNumber},
    {"getUserLang", &Os::getUserLang},
    {"getOsLang", &Os::getOsLang},
    {"getOsColor", &Os::getOsColor},
    {"showItemInFolder", &Os::showItemInFolder},
    {"openFile", &Os::openFile},
    };
    typedef LONG(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
}

Os::Os()
{
}

Os::~Os()
{
}

Os* Os::get()
{
    if(!os) {
        os = std::make_unique<Os>();
	}
    return os.get();
}
bool Os::execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Os::get()->*it->second)(param, result);
    return true;
}
void Os::getVersion(const rapidjson::Value& params, JsonResult* result) 
{
    RTL_OSVERSIONINFOW osInfo = { 0 };
    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
    HMODULE hNtDll = GetModuleHandle(L"ntdll.dll");
    if (hNtDll == NULL) {
        result->addErr("Failed to load ntdll.dll");
        return;
    }
    RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hNtDll, "RtlGetVersion");
    if (RtlGetVersion == NULL) {
        result->addErr("Failed to get RtlGetVersion function");
        return;
    }
    if (RtlGetVersion(&osInfo) != 0) {
        result->addErr("Failed to get version information");
        return;
    }
    if (osInfo.dwMajorVersion == 10 && osInfo.dwMinorVersion == 0) {
        if (osInfo.dwBuildNumber >= 22000) {
            result->addString("versionName", "win11");
        }
        else if (osInfo.dwBuildNumber >= 10240) {
            result->addString("versionName", "win10");
        }
    }
    result->addNumber("major", (long long)osInfo.dwMajorVersion);
    result->addNumber("minor", (long long)osInfo.dwMinorVersion);
    result->addNumber("build", (long long)osInfo.dwBuildNumber);
}

void Os::createShortcut(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto srcPath = arr[0].GetString();
    auto srcPathW = Util::convertToWStr(srcPath);
    auto dstPath = arr[1].GetString();
    auto dstPathW = Util::convertToWStr(dstPath);
    auto des = arr[2].GetString();
    auto desW = Util::convertToWStr(des);
    auto workDir = arr[3].GetString();
    auto workDirW = Util::convertToWStr(workDir);

    IShellLink* pShellLink = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pShellLink);
    if (SUCCEEDED(hr)) {
        pShellLink->SetPath(srcPathW.c_str());
        pShellLink->SetDescription(desW.c_str());
        pShellLink->SetIconLocation(srcPathW.c_str(), 0);
        pShellLink->SetWorkingDirectory(workDirW.c_str());
        IPersistFile* pPersistFile = nullptr;
        hr = pShellLink->QueryInterface(IID_IPersistFile, (LPVOID*)&pPersistFile);
        if (SUCCEEDED(hr)) {
            pPersistFile->Save(dstPathW.c_str(), TRUE);
            pPersistFile->Release();
        }
        pShellLink->Release();
    }
}

void Os::getCPUID(const rapidjson::Value& params, JsonResult* result)
{
    int cpuInfo[4] = { 0 };
    __cpuid(cpuInfo, 1);
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(8) << cpuInfo[0] << std::setw(8) << cpuInfo[3];
    auto str = ss.str();
    result->addString("data", str.data());
}

void Os::getDiskSerialNumber(const rapidjson::Value& params, JsonResult* result)
{
    std::string data;
    char volumeName[MAX_PATH + 1] = { 0 };
    char fileSystemName[MAX_PATH + 1] = { 0 };
    DWORD serialNumber = 0;
    DWORD maxComponentLen = 0;
    DWORD fileSystemFlags = 0; 
    char windowsPath[MAX_PATH] = { 0 };
    if (GetWindowsDirectoryA(windowsPath, MAX_PATH) == 0) {
        result->addErr("Failed to get Windows directory.");
        return;
    }
    std::string rootPath = std::string(windowsPath).substr(0, 3);
    if (GetVolumeInformationA(
        rootPath.c_str(),
        volumeName,
        MAX_PATH + 1,
        &serialNumber,
        &maxComponentLen,
        &fileSystemFlags,
        fileSystemName,
        MAX_PATH + 1)) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(8) << serialNumber;
        data = ss.str();
    }
    result->addString("data", data.data());
}

void Os::getUserLang(const rapidjson::Value& params, JsonResult* result)
{
    LANGID langID = GetUserDefaultLangID();
    WCHAR langName[LOCALE_NAME_MAX_LENGTH];
    if (!LCIDToLocaleName(MAKELCID(langID, SORT_DEFAULT), langName, LOCALE_NAME_MAX_LENGTH, 0)) {
        result->addErr("get lang name err");
        return;
    }
    auto str = Util::convertToStr(langName);
    result->addString("data", str.data());
}

void Os::getOsLang(const rapidjson::Value& params, JsonResult* result)
{
    LANGID langID = GetSystemDefaultLangID();
    WCHAR langName[LOCALE_NAME_MAX_LENGTH];
    if (!LCIDToLocaleName(MAKELCID(langID, SORT_DEFAULT), langName, LOCALE_NAME_MAX_LENGTH, 0)) {
        result->addErr("get lang name err");
        return;
    }
    auto str = Util::convertToStr(langName);
    result->addString("data", str.data());
}

void Os::getOsColor(const rapidjson::Value& params, JsonResult* result)
{
    COLORREF winColor = GetSysColor(COLOR_WINDOW);
    COLORREF btnColor = GetSysColor(CTLCOLOR_BTN);
    COLORREF bgColor = GetSysColor(COLOR_BACKGROUND);
    COLORREF textColor = GetSysColor(COLOR_WINDOWTEXT);
    COLORREF captionColor = GetSysColor(COLOR_CAPTIONTEXT);
    COLORREF borderColor = GetSysColor(COLOR_ACTIVEBORDER);
    auto winColorStr = Util::colorToHex(winColor);
    auto btnColorStr = Util::colorToHex(btnColor);
    auto bgColorStr = Util::colorToHex(bgColor);
    auto textColorStr = Util::colorToHex(textColor);
    auto captionColorStr = Util::colorToHex(captionColor);
    auto borderColorStr = Util::colorToHex(borderColor);
    result->addString("winColor", winColorStr.data());
    result->addString("btnColor", btnColorStr.data());
    result->addString("bgColor", bgColorStr.data());
    result->addString("textColor", textColorStr.data());
    result->addString("captionColor", captionColorStr.data());
    result->addString("borderColor", borderColorStr.data());
}

void Os::showItemInFolder(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto filePath = arr[0].GetString();
    auto str = Util::convertToWStr(filePath);
    std::wstring command = std::format(L"/select,\"{}\"",str);
    ShellExecute(nullptr, L"open", L"explorer.exe", command.c_str(), nullptr, SW_SHOW);
}

void Os::openFile(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto filePath = arr[0].GetString();
    auto str = Util::convertToWStr(filePath);
    HINSTANCE hr = ShellExecuteW(nullptr, L"open", str.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    result->addBool("data", (intptr_t)hr > 32);
}

// 保存数据到用户凭据区
// https://zhuanlan.zhihu.com/p/679219628?share_code=NzSd6ri8efPP&utm_psn=1912465887958639333

// 获取当前用户的皮肤颜色 Theme

//阻止锁屏

//插入电源，使用电池

//getSystemIdleTime

//遍历窗口句柄