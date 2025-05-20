#include "AppConfig.h"

AppConfig::AppConfig()
{
    auto jsonData = Util::readFile(L"UI/config.json");
    jsonDoc.Parse(jsonData.data());
    if (jsonDoc.HasMember("appId") && jsonDoc["appId"].IsString())
    {
        appId = jsonDoc["appId"].GetString();
    }
    if (jsonDoc.HasMember("quitWhenAllWindowClosed") && jsonDoc["quitWhenAllWindowClosed"].IsBool())
    {
        quitWhenAllWindowClosed = jsonDoc["quitWhenAllWindowClosed"].GetBool();
    }
}

AppConfig::~AppConfig()
{
}

rapidjson::Value& AppConfig::getFirstWindowConfig()
{
    return jsonDoc["window"];
}

void AppConfig::releaseJsonDoc()
{
    jsonDoc.SetObject(); // 清空文档，变成一个空对象
    jsonDoc.GetAllocator().Clear(); // 清空内存池
}
