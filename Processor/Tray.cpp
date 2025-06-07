#include <pch.h>
#include "Tray.h"
#include "../App/App.h"
#include "../App/BrowserWindow.h"
#include "../App/MenuWindow.h"

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

    auto win = result->getWin(); //todo Ïú»Ù tray
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

    
    if(config.HasMember("menu") && config["menu"].IsArray()){
        const rapidjson::Value::ConstArray menuArr = config["menu"].GetArray();
        std::map<int, std::wstring> menus;
        for (size_t i = 0; i < menuArr.Size(); i++)
        {
            const rapidjson::Value& value = menuArr[i];
            auto id = value["__id"].GetInt();
            auto text = Util::convertToWStr(value["text"].GetString());
            menus.insert({ id,text });
        }
        win->trayMenus.insert({ tray->uID ,std::move(menus)});
    }
}
