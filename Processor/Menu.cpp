#include <pch.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>


#include "Menu.h"

namespace {
    std::unique_ptr<Menu> menu;
    static std::unordered_map<std::string, void (Menu::*)(const rapidjson::Value&, JsonResult*)> funcs{
    {"create", &Menu::create},
    };
}

Menu::Menu()
{
}

Menu::~Menu()
{
}

Menu* Menu::get()
{
    if(!menu) {
        menu = std::make_unique<Menu>();
	}
    return menu.get();
}
bool Menu::excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Menu::get()->*it->second)(param, result);
    return true;
}
void Menu::create(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto menu = CreatePopupMenu();
    for (size_t i = 0; i < arr.Size(); i++)
    {
        const rapidjson::Value& value = arr[0];
        auto id = value["__id"].GetInt();
        auto text = Util::convertToWStr(value["text"].GetString());
        AppendMenu(menu, MF_STRING, id, text.data());
    }
}
