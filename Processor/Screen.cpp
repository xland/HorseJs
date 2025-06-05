#include <pch.h>
#include "Screen.h"

namespace {
    std::unique_ptr<Screen> screen;
    static std::unordered_map<std::string, void (Screen::*)(const rapidjson::Value&, JsonResult*)> screenFunc{
    };
}

Screen::Screen()
{
}

Screen::~Screen()
{
}

Screen* Screen::get()
{
    if(!screen) {
        screen = std::make_unique<Screen>();
	}
    return screen.get();
}
bool Screen::excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = screenFunc.find(methodName);
    if (it == screenFunc.end()) return false;
    (Screen::get()->*it->second)(param, result);
    return true;
}
//获取屏幕上某个点的颜色
//获取分辨率和DPI
//