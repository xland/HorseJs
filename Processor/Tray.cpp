#include <pch.h>
#include "Tray.h"
#include "../App/BrowserWindow.h"

namespace {
    std::unique_ptr<Tray> tray;
    std::unordered_map<int, std::vector<JsonResult*>> events;
    static std::unordered_map<std::string, void (Tray::*)(const rapidjson::Value&, JsonResult*)> trayFunc{
        {"create", &Tray::create},
    };
}

Tray::Tray()
{
}

Tray::~Tray()
{
}

Tray* Tray::get()
{
    if(!tray) {
        tray = std::make_unique<Tray>();
	}
    return tray.get();
}
bool Tray::excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = trayFunc.find(methodName);
    if (it == trayFunc.end()) return false;
    (Tray::get()->*it->second)(param, result);
    return true;
}

void Tray::create(const rapidjson::Value& params, JsonResult* result)
{
    static NOTIFYICONDATA nid = { 0 }; //全局的
    static HMENU hMenu = nullptr;
    static const UINT ID_TRAY_EXIT = 1001;

    static unsigned id = 0;


    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = result->getTar()->hwnd;
    nid.uID = id;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAY;
    nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcscpy_s(nid.szTip, L"System Tray Icon Demo");
    Shell_NotifyIcon(NIM_ADD, &nid);
    hMenu = CreatePopupMenu();
    
    AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");
}
