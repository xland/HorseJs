#include "BrowserWindowConfig.h"

BrowserWindowConfig::BrowserWindowConfig(const rapidjson::Value& config)
{
    if (config.HasMember("size"))
    {
        if (config["size"].IsString())
        {
            auto size = std::string_view(config["size"].GetString());
            if (size == "maximize")
            {
                maximize = true;
            }
        }
        else if (config["size"].IsObject())
        {
            auto sizeObj = config["size"].GetObj();
            w = sizeObj["w"].GetInt();
            h = sizeObj["h"].GetInt();
        }
    }
    if (config.HasMember("position") && config["position"].IsString())
    {
        auto pos = std::string_view(config["position"].GetString());
        if (pos == "screenCenter")
        {
            int sw = GetSystemMetrics(SM_CXSCREEN);
            int sh = GetSystemMetrics(SM_CYSCREEN);
            x = (sw - w) / 2;
            y = (sh - h) / 2;
        }
    }
    if (config.HasMember("visible") && config["visible"].IsBool())
    {
        visible = config["visible"].GetBool();
    }
    if (config.HasMember("frame") && config["frame"].IsBool())
    {
        frame = config["frame"].GetBool();
    }
    if (config.HasMember("shadow") && config["shadow"].IsBool())
    {
        shadow = config["shadow"].GetBool();
    }
    if (config.HasMember("maximizable") && config["maximizable"].IsBool())
    {
        maximizable = config["maximizable"].GetBool();
    }
    if (config.HasMember("title") && config["title"].IsString())
    {
        title = Util::convertToWStr(config["title"].GetString());
    }
    if (config.HasMember("resizable") && config["resizable"].IsBool())
    {
        resizable = config["resizable"].GetBool();
    }
}

BrowserWindowConfig::~BrowserWindowConfig()
{
}
