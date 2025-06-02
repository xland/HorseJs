#include <pch.h>
#include "Horse.h"
#include "../App/App.h"
#include "../Win/BrowserWindow.h"
#include "../Win/BrowserWindowConfig.h"
#include "../Win/Page.h"

namespace {
    std::unique_ptr<Horse> horse;
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

void Horse::getConfig(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto str = Util::readFile(L"UI/config.json");
    rapidjson::Document doc;
    doc.Parse(str.data());
    rapidjson::Value copiedValue(doc, result->getAllocator());
	result->addValue("result", std::move(copiedValue));
    result->returnBack();
}

void Horse::createWindow(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    const rapidjson::Value& value = arr[0];
    auto winIns = std::make_unique<BrowserWindow>(value);
    if (value.HasMember("page") && value["page"].IsObject()) {
        const rapidjson::Value& pageObject = value["page"];
        winIns->load(pageObject);
    }
    result->addNumber("id", winIns->config->id);
    App::get()->winMap.insert({ winIns->config->id,std::move(winIns) });
    result->returnBack();
}
