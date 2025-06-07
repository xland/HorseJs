#include <pch.h>
#include "Horse.h"
#include "../App/App.h"
#include "../App/BrowserWindow.h"

namespace {
    std::unique_ptr<Horse> horse;
    static std::unordered_map<std::string, void (Horse::*)(const rapidjson::Value&, JsonResult*)> funcs{
    {"getConfig", &Horse::getConfig},
    {"createWindow", &Horse::createWindow},
    };
}

Horse::Horse()
{
}

Horse::~Horse()
{
}

Horse* Horse::get()
{
    if(!horse) {
        horse = std::make_unique<Horse>();
	}
    return horse.get();
}
bool Horse::excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Horse::get()->*it->second)(param, result);
    return true;
}
void Horse::getConfig(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto str = Util::readFile(L"UI/config.json");
    rapidjson::Document doc;
    doc.Parse(str.data());
    rapidjson::Value copiedValue(doc, result->getAllocator());
	result->addValue("data", std::move(copiedValue));
}

void Horse::createWindow(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    const rapidjson::Value& value = arr[0];
    auto winIns = std::make_unique<BrowserWindow>(value);
    winIns->load();
    result->addNumber("id", winIns->id);
    App::get()->winMap.insert({ winIns->id,std::move(winIns) });
}
