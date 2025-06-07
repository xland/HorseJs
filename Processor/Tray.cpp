#include <pch.h>
#include "Tray.h"
#include "../App/BrowserWindow.h"

namespace {
    std::unique_ptr<Tray> tray;
    static std::unordered_map<std::string, void (Tray::*)(const rapidjson::Value&, JsonResult*)> funcs{
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
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Tray::get()->*it->second)(param, result);
    return true;
}

void Tray::create(const rapidjson::Value& params, JsonResult* result)
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
    wcscpy_s(tray->szTip, L"System Tray Icon Demo");
    Shell_NotifyIcon(NIM_ADD, tray);
    win->trays.push_back(tray);

    if(config.HasMember("menu") && config["menu"].IsArray()){
        HMENU menu = CreatePopupMenu();    
        const rapidjson::Value::ConstArray menuArr = config["menu"].GetArray();
        for (size_t i = 0; i < menuArr.Size(); i++)
        {
            const rapidjson::Value& value = menuArr[i];
            auto id = value["__id"].GetInt();
            auto text = Util::convertToWStr(value["text"].GetString());
            AppendMenu(menu, MF_STRING, id, text.data());
        }
        win->menus.insert({ tray->uID,std::move(menu) });
    }
}
