#include <pch.h>
#include "Os.h"

namespace {
    std::unique_ptr<Os> os;
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

void Os::getVersion(const rapidjson::Value& params, JsonResult* result) 
{
    RTL_OSVERSIONINFOW osInfo = { 0 };
    osInfo.dwOSVersionInfoSize = sizeof(osInfo);

    // 动态加载 ntdll.dll 中的 RtlGetVersion
    HMODULE hNtDll = GetModuleHandleW(L"ntdll.dll");
    if (hNtDll == NULL) {
        printf("Failed to load ntdll.dll\n");
        return;
    }

    RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hNtDll, "RtlGetVersion");
    if (RtlGetVersion == NULL) {
        printf("Failed to get RtlGetVersion function\n");
        return;
    }

    // 调用 RtlGetVersion 获取版本信息
    if (RtlGetVersion(&osInfo) == 0) { // 0 表示成功
        printf("OS Version: %lu.%lu, Build: %lu\n", osInfo.dwMajorVersion, osInfo.dwMinorVersion, osInfo.dwBuildNumber);

        // 判断 Windows 10 或 Windows 11
        if (osInfo.dwMajorVersion == 10 && osInfo.dwMinorVersion == 0) {
            if (osInfo.dwBuildNumber >= 22000) {
                printf("Current System: Windows 11\n");
            }
            else if (osInfo.dwBuildNumber >= 10240) {
                printf("Current System: Windows 10\n");
            }
            else {
                printf("Unknown Windows version\n");
            }
        }
        else {
            printf("Not Windows 10 or Windows 11\n");
        }
    }
    else {
        printf("Failed to get version information\n");
    }
}

// 保存数据到用户凭据区
// https://zhuanlan.zhihu.com/p/679219628?share_code=NzSd6ri8efPP&utm_psn=1912465887958639333

// 获取当前用户的皮肤颜色 Theme

//阻止锁屏

//插入电源，使用电池

//getSystemIdleTime

//遍历窗口句柄