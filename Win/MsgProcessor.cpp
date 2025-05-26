#include <memory>

#include "MsgProcessor.h"
#include "EnumId.h"
#include "BrowserWindow.h"
#include "Page.h"
#include "../App/Fs.h"
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
    else if (classId == (int)ClassId::Fs) {
        processFs(methodId, jsonDoc["params"], parsor);
    }
    
    

    //rapidjson::Value items(rapidjson::kArrayType);
    //rapidjson::Value number1(42); // 第一个数字
    //rapidjson::Value number2(3.14); // 第二个数字（支持浮点数）
    //items.PushBack(number1, parsor.getAllocator());
    //items.PushBack(number2, parsor.getAllocator());
    //parsor.addValue("param", std::move(items));
    // 
    //返回值，必须放在后面，因为win有可能被销毁
    if (!this->win || !this->page) return;
    std::wstring jsonStr = parsor.parse();
    page->webview->PostWebMessageAsJson(jsonStr.data());
}

void MsgProcessor::emit(const int& classId, const int& eventId, int count, ...)
{
    if (!win || !page) return;
    JsonParsor parsor;
    parsor.addNumber("classId", classId);
    parsor.addNumber("eventId", eventId);
    va_list args;
    va_start(args, count);
    if (classId == (int)ClassId::Window) {
        emitWin(eventId, count, args,parsor);
    }
    else if (classId == (int)ClassId::Page) {
        emitPage(eventId, count, args,parsor);
    }
    va_end(args);
    std::wstring jsonStr = parsor.parse();
    page->webview->PostWebMessageAsJson(jsonStr.data());
}

void MsgProcessor::emitWin(const int& eventId, int count, va_list args, JsonParsor& parsor)
{
    if (eventId == (int)WindowEventId::sizePosChanged) {
        int x = va_arg(args, int);
        int y = va_arg(args, int);
        int w = va_arg(args, int);
        int h = va_arg(args, int);
        rapidjson::Value items(rapidjson::kArrayType);
        items.PushBack(rapidjson::Value(x), parsor.getAllocator());
        items.PushBack(rapidjson::Value(y), parsor.getAllocator());
        items.PushBack(rapidjson::Value(w), parsor.getAllocator());
        items.PushBack(rapidjson::Value(h), parsor.getAllocator());
        parsor.addValue("param", std::move(items));
    }
}

void MsgProcessor::emitPage(const int& msgId, int count, va_list args, JsonParsor& parsor)
{
}

void MsgProcessor::processPage(const int& methodId, const rapidjson::Value& params, JsonParsor& parsor)
{
    const rapidjson::Value::ConstArray paramsArray = params.GetArray();
    if (methodId == (int)WindowMethodId::maximize) {

    }
    else if (methodId == (int)WindowMethodId::minimize) {

    }
    else if (methodId == (int)WindowMethodId::resize) {
        auto w = paramsArray[0].GetInt();
        auto h = paramsArray[1].GetInt();
        win->resize(w, h);
    }
    else if (methodId == (int)WindowMethodId::move) {

    }
}

void MsgProcessor::processWin(const int& methodId, const rapidjson::Value& params, JsonParsor& parsor)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    if (methodId == (int)WindowMethodId::maximize) {
        win->maximize();
    }
    else if (methodId == (int)WindowMethodId::minimize) {
        win->minimize();
    }
    else if (methodId == (int)WindowMethodId::show) {
        win->show();
    }
    else if (methodId == (int)WindowMethodId::hide) {
        win->hide();
    }
    else if (methodId == (int)WindowMethodId::restore) {
        win->restore();
    }
    else if (methodId == (int)WindowMethodId::resize) {
        auto w = arr[0].GetInt();
        auto h = arr[1].GetInt();
        win->resize(w, h);
    }
    else if (methodId == (int)WindowMethodId::move) {

    }
    else if (methodId == (int)WindowMethodId::close) {
        win->close();
    }
    else if (methodId == (int)WindowMethodId::destory) {
        win->destroy();
    }
    else if (methodId == (int)WindowMethodId::regEvent) {
        auto eventId = arr[0].GetInt();
        win->regEvent(eventId);
    }
    else if (methodId == (int)WindowMethodId::unregEvent) {
        auto eventId = arr[0].GetInt();
        win->unregEvent(eventId);
    }
}

void MsgProcessor::processFs(const int& methodId, const rapidjson::Value& params, JsonParsor& parsor)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    if (methodId == (int)FsMethodId::addResToExe) {
        auto str1 = arr[0].GetString();
        auto str2 = arr[1].GetString();
        auto dirPath = Util::convertToWStr(str1);
        auto exePath = Util::convertToWStr(str2);
        Fs::get()->addDirAsExeRes(dirPath, exePath);
    }
}
