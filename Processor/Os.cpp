#include <pch.h>
#include "Os.h"

namespace {
    std::unique_ptr<Os> os;
    static std::unordered_map<std::string, void (Os::*)(const rapidjson::Value&, JsonResult*)> funcs{
    {"getVersion", &Os::getVersion},
    {"createShortcut", &Os::createShortcut},
    };
    typedef LONG(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
    //todo 获取磁盘、主板、网卡串号
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

// 保存数据到用户凭据区
// https://zhuanlan.zhihu.com/p/679219628?share_code=NzSd6ri8efPP&utm_psn=1912465887958639333

// 获取当前用户的皮肤颜色 Theme

//阻止锁屏

//插入电源，使用电池

//getSystemIdleTime

//遍历窗口句柄