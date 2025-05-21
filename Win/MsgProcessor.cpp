#include "MsgProcessor.h"
#include "EnumId.h"
#include "BrowserWindow.h"
#include "Page.h"
#include "../App/JsonParsor.h"

MsgProcessor::MsgProcessor(BrowserWindow* win, Page* page) :win{ win }, page{page}
{
}

MsgProcessor::~MsgProcessor()
{
}

void MsgProcessor::processStr(const std::string& msgStr)
{
    rapidjson::Document jsonDoc;
    jsonDoc.Parse(msgStr.data());
    int classId{-1}, methodId{ -1 }, eventId{ -1 };
    if (jsonDoc.HasMember("classId") && jsonDoc["classId"].IsInt()) {
        classId = jsonDoc["classId"].GetInt();
    }
    if (jsonDoc.HasMember("methodId") && jsonDoc["methodId"].IsInt()) {
        methodId = jsonDoc["methodId"].GetInt();
    }
    if (jsonDoc.HasMember("eventId") && jsonDoc["eventId"].IsInt()) {
        eventId = jsonDoc["eventId"].GetInt();
    }
    if (classId < 0 || methodId < 0 || eventId < 0) {
        MessageBox(nullptr, L"classId,methodId或eventId为空", L"错误", MB_OK | MB_ICONERROR);
        return;
    }
    JsonParsor parsor;
    parsor.addNumber("eventId", eventId);
    if (classId == (int)ClassId::Window) {
        processWin(methodId, jsonDoc["params"],parsor);
    }
    else if (classId == (int)ClassId::Page) {
        processPage(methodId, jsonDoc["params"],parsor);
    }    
    //rapidjson::Value items(rapidjson::kArrayType);
    //rapidjson::Value number1(42); // 第一个数字
    //rapidjson::Value number2(3.14); // 第二个数字（支持浮点数）
    //items.PushBack(number1, parsor.getAllocator());
    //items.PushBack(number2, parsor.getAllocator());
    //parsor.addValue("param", std::move(items));
    std::wstring jsonStr = parsor.parse();
    page->webview->PostWebMessageAsJson(jsonStr.data());
}

void MsgProcessor::processPage(const int& methodId, const rapidjson::Value& params, JsonParsor& parsor)
{
}

void MsgProcessor::processWin(const int& methodId, const rapidjson::Value& params, JsonParsor& parsor)
{
    const rapidjson::Value::ConstArray paramsArray = params.GetArray();
    if (methodId == (int)WindowMethodId::resize) {        
        auto w = paramsArray[0].GetInt();
        auto h = paramsArray[1].GetInt();
        win->resize(w,h);
    }
}
