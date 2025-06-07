#include <pch.h>
#include "Tray.h"
#include "../App/BrowserWindow.h"
#include "../App/HelperWindow.h"

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
    auto trayData = new TrayData();
    trayData->id = config["__id"].GetInt();
    trayData->winId = result->winId;
    if (config.HasMember("tip") && config["tip"].IsString()) {
        trayData->tip = Util::convertToWStr(config["tip"].GetString());
    }
    if(config.HasMember("menu") && config["menu"].IsArray()){
        const rapidjson::Value::ConstArray menuArr = config["menu"].GetArray();
        for (size_t i = 0; i < menuArr.Size(); i++)
        {
            const rapidjson::Value& value = menuArr[i];
            auto id = value["__id"].GetInt();
            auto text = Util::convertToWStr(value["text"].GetString());
            trayData->menus.insert({ id,text });
        }
    }
    HelperWindow::get()->startCreateTray(trayData);
}
