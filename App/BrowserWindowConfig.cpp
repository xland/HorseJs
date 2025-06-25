#include <pch.h>
#include "BrowserWindow.h"

void BrowserWindow::configOther(const rapidjson::Value& config)
{
    static int orgId{ 0 };
    id = orgId;
    orgId += 1;
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
    if (config.HasMember("minimizable") && config["minimizable"].IsBool())
    {
        minimizable = config["minimizable"].GetBool();
    }
    if (config.HasMember("alwaysOnTop") && config["alwaysOnTop"].IsBool())
    {
        alwaysOnTop = config["alwaysOnTop"].GetBool();
    }
    if (config.HasMember("skipTaskbar") && config["skipTaskbar"].IsBool())
    {
        skipTaskbar = config["skipTaskbar"].GetBool();
    }
    if (config.HasMember("resizable") && config["resizable"].IsBool())
    {
        resizable = config["resizable"].GetBool();
    }
    if (config.HasMember("title") && config["title"].IsString())
    {
        title = Util::convertToWStr(config["title"].GetString());
    }
}

void BrowserWindow::configSize(const rapidjson::Value& config)
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

void BrowserWindow::configPos(const rapidjson::Value& config)
{
    if (config.HasMember("pos"))
    {
        if(config["pos"].IsString()){
            std::string pos = config["pos"].GetString();
            if (pos == "centerScreen")
            {
                RECT workArea;
                SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
                x = workArea.left + (workArea.right - workArea.left - w) / 2;
                y = workArea.top + (workArea.bottom - workArea.top - h) / 2;
            }
        }
        else if (config["pos"].IsObject()) {
            auto posObj = config["pos"].GetObj();
            x = posObj["x"].GetInt();
            y = posObj["y"].GetInt();
        }
    }
}
void BrowserWindow::configPage(const rapidjson::Value& config)
{
    if (config.HasMember("scriptDialogEnable") && config["scriptDialogEnable"].IsBool())
    {
        scriptDialogEnable = config["scriptDialogEnable"].GetBool();
    }
    if (config.HasMember("webMessageEnable") && config["webMessageEnable"].IsBool())
    {
        webMessageEnable = config["webMessageEnable"].GetBool();
    }
    if (config.HasMember("scriptEnable") && config["scriptEnable"].IsBool())
    {
        scriptEnable = config["scriptEnable"].GetBool();
    }
    if (config.HasMember("contextMenuEnable") && config["contextMenuEnable"].IsBool())
    {
        contextMenuEnable = config["contextMenuEnable"].GetBool();
    }
    if (config.HasMember("url") && config["url"].IsString())
    {
        url = Util::convertToWStr(config["url"].GetString());
    }
}
