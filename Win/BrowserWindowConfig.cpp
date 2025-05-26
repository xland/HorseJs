#include "BrowserWindowConfig.h"

BrowserWindowConfig::BrowserWindowConfig(const rapidjson::Value& config)
{
    setSize(config);
    setPos(config);
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

    if (config.HasMember("title") && config["title"].IsString())
    {
        title = Util::convertToWStr(config["title"].GetString());
    }
}

BrowserWindowConfig::~BrowserWindowConfig()
{
}

void BrowserWindowConfig::setSize(const rapidjson::Value& config)
{
    if (config.HasMember("maximizable") && config["maximizable"].IsBool())
    {
        maximizable = config["maximizable"].GetBool();
    }
    if (config.HasMember("resizable") && config["resizable"].IsBool())
    {
        resizable = config["resizable"].GetBool();
    }
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
    if (config.HasMember("minSize"))
    {
        if (config["minSize"].IsObject())
        {
            auto sizeObj = config["minSize"].GetObj();
            minWidth = sizeObj["w"].GetInt();
            minHeight = sizeObj["h"].GetInt();
        }
    }
    if (config.HasMember("maxSize"))
    {
        if (config["maxSize"].IsObject())
        {
            auto sizeObj = config["maxSize"].GetObj();
            maxWidth = sizeObj["w"].GetInt();
            maxHeight = sizeObj["h"].GetInt();
            maximizable = false;
        }
    }
}

void BrowserWindowConfig::setPos(const rapidjson::Value& config)
{
    if (config.HasMember("position"))
    {
        if(config["position"].IsString()){
            auto pos = std::string_view(config["position"].GetString());
            if (pos == "screenCenter")
            {
                int sw = GetSystemMetrics(SM_CXSCREEN);
                int sh = GetSystemMetrics(SM_CYSCREEN);
                x = (sw - w) / 2;
                y = (sh - h) / 2;
            }
        }
        else if (config["size"].IsObject()) {
            auto posObj = config["position"].GetObj();
            x = posObj["x"].GetInt();
            y = posObj["y"].GetInt();
        }
    }
}
