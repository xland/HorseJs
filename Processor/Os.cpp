#include <pch.h>
#include "../App/BrowserWindow.h"
#include "Os.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <netlistmgr.h>
#include <comdef.h>
#include <wtsapi32.h>

#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include "../App/App.h"
#include "NetConnListener.h"

namespace {
    std::unique_ptr<Os> os;
    static std::unordered_map<std::string, void (Os::*)(const rapidjson::Value&, JsonResult*)> funcs{
    {"getOsVer", &Os::getOsVer},
    {"getCpuId", &Os::getCpuId},
    {"getDiskId", &Os::getDiskId},
    {"getUserLang", &Os::getUserLang},
    {"getOsLang", &Os::getOsLang},
    {"getOsColor", &Os::getOsColor},
    {"preventSleep", &Os::preventSleep},
    {"stopPreventSleep", &Os::stopPreventSleep},
    {"getIpAddr", &Os::getIpAddr},
    {"showNotify", &Os::showNotify},
    {"createTray", &Os::createTray},
    {"destroyTray", &Os::destroyTray},
    {"credWrite", &Os::credWrite},
    {"credRead", &Os::credRead},
    {"credDel", &Os::credDel},
    {"spawn", &Os::spawn},
    {"on", &Os::on},
    {"off", &Os::off},
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
void Os::getOsVer(const rapidjson::Value& params, JsonResult* result)
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



void Os::getCpuId(const rapidjson::Value& params, JsonResult* result)
{
    int cpuInfo[4] = { 0 };
    __cpuid(cpuInfo, 1);
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(8) << cpuInfo[0] << std::setw(8) << cpuInfo[3];
    auto str = ss.str();
    result->addString("data", str.data());
}

void Os::getDiskId(const rapidjson::Value& params, JsonResult* result)
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


void Os::preventSleep(const rapidjson::Value& params, JsonResult* result)
{
    SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED | ES_CONTINUOUS);
}

void Os::stopPreventSleep(const rapidjson::Value& params, JsonResult* result)
{
    SetThreadExecutionState(ES_CONTINUOUS);
}


void Os::on(const rapidjson::Value& params, JsonResult* result) 
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::string eventName = arr[0].GetString();
    auto win = result->getWin();
    if (eventName == "osLock") {
        if (!WTSRegisterSessionNotification(win->hwnd, NOTIFY_FOR_THIS_SESSION)) {
            result->addErr("listen os lock err");
            return;
        }
    }
    else if (eventName == "connChanged") {
        if (!NetConnListener::listen(result->winId)) {
            result->addErr("can not listen network");
            return;
        }
    }
    win->events[eventName].insert(result->winId);
}
void Os::off(const rapidjson::Value& params, JsonResult* result) 
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::string eventName = arr[0].GetString();
    auto win = result->getWin();
    win->events[eventName].erase(result->winId);
    if (eventName == "osLock") {
        if (!WTSUnRegisterSessionNotification(win->hwnd)) {
            result->addErr("unlisten os lock err");
            return;
        }
    } else if (eventName == "connChanged") {
        if (!NetConnListener::listen(result->winId)) {
            result->addErr("can not listen network");
            return;
        }
    }
}

void Os::getIpAddr(const rapidjson::Value& params, JsonResult* result)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        result->addErr("WSAStartup failed");
        return;
    }
    // 设置初始缓冲区大小
    ULONG bufferSize = 15000;
    std::vector<char> buffer(bufferSize);
    PIP_ADAPTER_ADDRESSES adapters = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());

    // 获取适配器信息,支持 IPv4 和 IPv6
    DWORD dr = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr, adapters, &bufferSize);
    if (dr == ERROR_BUFFER_OVERFLOW) {
        buffer.resize(bufferSize);
        adapters = reinterpret_cast<PIP_ADAPTER_ADDRESSES>(buffer.data());
        dr = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr, adapters, &bufferSize);
    }
    if (dr != ERROR_SUCCESS) {
        result->addErr("GetAdaptersAddresses failed");
        WSACleanup();
        return;
    }

    rapidjson::Value array(rapidjson::kArrayType);
    for (PIP_ADAPTER_ADDRESSES adapter = adapters; adapter; adapter = adapter->Next) {
        // 跳过未启用的适配器
        if (adapter->OperStatus != IfOperStatusUp) {
            continue;
        }

        // 创建适配器对象
        rapidjson::Value adapterObj(rapidjson::kObjectType);

        // 获取适配器名称
        std::wstring wideName = adapter->FriendlyName;
        std::string adapterName(wideName.begin(), wideName.end());
        rapidjson::Value nameVal;
        nameVal.SetString(adapterName.c_str(), adapterName.length(), result->getAllocator());
        adapterObj.AddMember("name", nameVal, result->getAllocator());

        // 获取物理地址（MAC地址）
        rapidjson::Value macVal;
        if (adapter->PhysicalAddressLength > 0) {
            char macStr[18]; // 格式为 XX-XX-XX-XX-XX-XX
            snprintf(macStr, sizeof(macStr), "%02X-%02X-%02X-%02X-%02X-%02X",
                adapter->PhysicalAddress[0], adapter->PhysicalAddress[1],
                adapter->PhysicalAddress[2], adapter->PhysicalAddress[3],
                adapter->PhysicalAddress[4], adapter->PhysicalAddress[5]);
            macVal.SetString(macStr, strlen(macStr), result->getAllocator());
        }
        else {
            continue;
        }
        adapterObj.AddMember("mac", macVal, result->getAllocator());

        // 获取IP地址
        rapidjson::Value ipArray(rapidjson::kArrayType);
        for (PIP_ADAPTER_UNICAST_ADDRESS unicast = adapter->FirstUnicastAddress; unicast; unicast = unicast->Next)
        {
            SOCKET_ADDRESS& sockAddr = unicast->Address;
            std::string addr;
            if (sockAddr.lpSockaddr->sa_family == AF_INET) { // IPv4
                char buffer[INET_ADDRSTRLEN];
                sockaddr_in* ipv4 = reinterpret_cast<sockaddr_in*>(sockAddr.lpSockaddr);
                addr = inet_ntop(AF_INET, &(ipv4->sin_addr), buffer, sizeof(buffer));
                if (addr == "127.0.0.1") {
                    continue;
                }
            }
            else if (sockAddr.lpSockaddr->sa_family == AF_INET6) { // IPv6
                char buffer[INET6_ADDRSTRLEN];
                sockaddr_in6* ipv6 = reinterpret_cast<sockaddr_in6*>(sockAddr.lpSockaddr);
                addr = inet_ntop(AF_INET6, &(ipv6->sin6_addr), buffer, sizeof(buffer));
                if (addr == "::1") {
                    continue;
                }
            }
            if (!addr.empty()) {
                rapidjson::Value val;
                val.SetString(addr.c_str(), addr.length(), result->getAllocator());
                ipArray.PushBack(val, result->getAllocator());
            }
        }
        adapterObj.AddMember("addresses", ipArray, result->getAllocator());
        array.PushBack(adapterObj, result->getAllocator());
    }

    WSACleanup();
    result->addValue("data", std::move(array));
}

void Os::showNotify(const rapidjson::Value& params, JsonResult* result)
{
    using namespace winrt;
    using namespace winrt::Windows::UI;
    using namespace winrt::Windows::Data::Xml::Dom;

    //auto aumid = std::format(L"com.liulun.{}", appIdStr.data());
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring title = Util::convertToWStr(arr[1].GetString());
    std::wstring content = Util::convertToWStr(arr[2].GetString());
    auto xmlString = std::format(L"<toast><visual><binding template='ToastText02'>"
        L"<text id='1'>{}</text><text id='2'>{}</text></binding></visual></toast>",
        title, content);
    XmlDocument xmlDoc;
    xmlDoc.LoadXml(xmlString);
    Notifications::ToastNotification toast(xmlDoc);
    Notifications::ToastNotifier notifier = Notifications::ToastNotificationManager::CreateToastNotifier(App::get()->appId.data());
    notifier.Show(toast);
}

void Os::createTray(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    const rapidjson::Value& config = arr[0];

    auto win = result->getWin();
    NOTIFYICONDATA* tray = new NOTIFYICONDATA();
    ZeroMemory(tray, sizeof(NOTIFYICONDATA));
    tray->cbSize = sizeof(NOTIFYICONDATA);
    tray->hWnd = win->hwnd;
    tray->uID = config["__id"].GetInt();
    tray->uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    tray->uCallbackMessage = WM_TRAY;
    tray->hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    if (config.HasMember("tip") && config["tip"].IsString()) {
        auto str = Util::convertToWStr(config["tip"].GetString());
        wcscpy_s(tray->szTip, str.data());
    }
    Shell_NotifyIcon(NIM_ADD, tray);
    win->trays.push_back(tray);
    if (config.HasMember("menu") && config["menu"].IsArray()) {
        HMENU menu = CreatePopupMenu();
        const rapidjson::Value::ConstArray menuArr = config["menu"].GetArray();
        for (size_t i = 0; i < menuArr.Size(); i++)
        {
            const rapidjson::Value& value = menuArr[i];
            auto id = value["__id"].GetInt();
            auto text = Util::convertToWStr(value["text"].GetString());
            AppendMenu(menu, MF_STRING, id, text.data());
        }
        win->trayMenus.insert({ tray->uID ,menu });
    }
	result->addNumber("id", (int)tray->uID);
}

void Os::destroyTray(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    int trayId = arr[0].GetInt();
    auto win = result->getWin();
    for (auto it = win->trays.begin(); it != win->trays.end(); ++it) {
        if ((*it)->uID == trayId) {
            Shell_NotifyIcon(NIM_DELETE, *it);
            delete *it;
            win->trays.erase(it);
            break;
        }
    }
    auto menuIt = win->trayMenus.find(trayId);
    if (menuIt != win->trayMenus.end()) {
        DestroyMenu(menuIt->second);
        win->trayMenus.erase(menuIt);
	}
}

void Os::credWrite(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring key = Util::convertToWStr(arr[0].GetString());
    std::wstring val = Util::convertToWStr(arr[1].GetString());
    CREDENTIAL cred = { 0 };
    cred.Type = CRED_TYPE_GENERIC;
    cred.TargetName = key.data();
    cred.CredentialBlobSize = val.size() * sizeof(wchar_t);
    cred.CredentialBlob = (LPBYTE)(val.data());
    cred.Persist = CRED_PERSIST_LOCAL_MACHINE;
    auto flag = CredWrite(&cred, 0);
    if(!flag) {
        result->addErr("can not write credential");
	}
}

void Os::credRead(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring key = Util::convertToWStr(arr[0].GetString());
    CREDENTIAL* cred;
    if (CredRead(key.data(), CRED_TYPE_GENERIC, 0, &cred) == FALSE) {
        result->addErr("can not read credential");
        return;
    }
    std::wstring wstr(reinterpret_cast<wchar_t*>(cred->CredentialBlob), cred->CredentialBlobSize);
    CredFree(cred);
	auto str = Util::convertToStr(wstr);
	result->addString("data", str.data());
}

void Os::credDel(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring key = Util::convertToWStr(arr[0].GetString());
    if (!CredDelete(key.data(), CRED_TYPE_GENERIC, 0)) {
        result->addErr("can not del credential");
    }
}


void Os::spawn(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring path = Util::convertToWStr(arr[0].GetString());

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    si.cb = sizeof(STARTUPINFO);
    //si.hStdError = hStdoutWrite;  // 标准错误重定向到输出管道
    //si.hStdOutput = hStdoutWrite; // 标准输出重定向到输出管道
    //si.hStdInput = hStdinRead;    // 标准输入重定向到输入管道
    //si.dwFlags |= STARTF_USESTDHANDLES; // 使用标准句柄

    if (!CreateProcess(NULL, path.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        result->addErr("can not create process");
    }
    //WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

// 保存数据到用户凭据区
// https://zhuanlan.zhihu.com/p/679219628?share_code=NzSd6ri8efPP&utm_psn=1912465887958639333

// 获取当前用户的皮肤颜色 Theme

//插入电源，使用电池

//getSystemIdleTime

//遍历窗口句柄