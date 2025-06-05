#include <pch.h>
#include "Tray.h"

namespace {
    std::unique_ptr<Tray> tray;
    static std::unordered_map<std::string, void (Tray::*)(const rapidjson::Value&, JsonResult*)> trayFunc{
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