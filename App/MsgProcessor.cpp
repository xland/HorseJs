#include <memory>

#include "MsgProcessor.h"
#include "App.h"
#include "Fs.h"
#include "JsonParsor.h"
#include "../Win/BrowserWindow.h"
#include "../Win/Page.h"

namespace {
    std::unique_ptr<MsgProcessor> msgProcessor;
    static std::unordered_map<std::string, void (BrowserWindow::*)(const rapidjson::Value&, JsonParsor&)> winFunc = {
        {"maximize", &BrowserWindow::maximize},
        {"show", &BrowserWindow::show},
        {"flash", &BrowserWindow::flash},
    };
    static std::unordered_map<std::string, void (BrowserWindow::*)(const rapidjson::Value&, JsonParsor&)> pageFunc = {
        {"maximize", &BrowserWindow::maximize},
    };
    static std::unordered_map<std::string, void (BrowserWindow::*)(const rapidjson::Value&, JsonParsor&)> fsFunc = {
        {"maximize", &BrowserWindow::maximize},
    };
}

void MsgProcessor::init() {
    msgProcessor = std::make_unique<MsgProcessor>();
}

MsgProcessor* MsgProcessor::get() {
    return msgProcessor.get();
}

MsgProcessor::MsgProcessor()
{
}

MsgProcessor::~MsgProcessor()
{
}

void MsgProcessor::processStr(const std::string& msgStr)
{    
    rapidjson::Document jsonDoc;
    jsonDoc.Parse(msgStr.data());
    std::string className, methodName, eventName;
    if (jsonDoc.HasMember("className") && jsonDoc["className"].IsString()) {
        className = jsonDoc["className"].GetString();
    }
    if (jsonDoc.HasMember("methodName") && jsonDoc["methodName"].IsString()) {
        methodName = jsonDoc["methodName"].GetString();
    }
    if (jsonDoc.HasMember("eventName") && jsonDoc["eventName"].IsString()) {
        eventName = jsonDoc["eventName"].GetString();
    }
    int tarId{ -1 }, srcId{ -1 };
    if (jsonDoc.HasMember("tarId") && jsonDoc["tarId"].IsInt()) {
        tarId = jsonDoc["tarId"].GetInt();
    }
    if (jsonDoc.HasMember("srcId") && jsonDoc["srcId"].IsInt()) {
        srcId = jsonDoc["srcId"].GetInt();
    }
    if (className.empty() || methodName.empty() || eventName.empty()) {
        MessageBox(nullptr, L"className,methodName,eventName,tarId,srcId为空", L"错误", MB_OK | MB_ICONERROR);
        return;
    }
    auto srcWin = App::get()->getWindow(srcId);
    auto tarWin = srcWin;
    if (srcId != tarId) {
        tarWin = App::get()->getWindow(tarId);
    }
    JsonParsor parsor;
    parsor.addString("className", className);
    parsor.addString("eventName", eventName);
    if (className == "horse") {
        //processHorse(methodName, jsonDoc["params"], parsor);
    }
    else if (className == "window") {
        auto it = winFunc.find(methodName);
        if (it != winFunc.end()) {
            (tarWin->*it->second)(jsonDoc["params"], parsor);
        }
        else {
            std::cout << "Method not found!" << std::endl;
        }
    }
    else if (className == "page") {
        //processPage(methodName, jsonDoc["params"],parsor);
    }
    else if (className == "fs") {
        //processFs(methodName, jsonDoc["params"], parsor);
    }
    //返回值，必须放在后面，因为win有可能被销毁
    std::wstring jsonStr = parsor.parse();
    srcWin->page->webview->PostWebMessageAsJson(jsonStr.data());
}

//void MsgProcessor::emit(const int& classId, const int& eventId, int count, ...)
//{
//    if (!win || !page) return;
//    JsonParsor parsor;
//    parsor.addNumber("classId", classId);
//    parsor.addNumber("eventId", eventId);
//    va_list args;
//    va_start(args, count);
//    if (classId == (int)ClassId::Window) {
//        emitWin(eventId, count, args,parsor);
//    }
//    else if (classId == (int)ClassId::Page) {
//        emitPage(eventId, count, args,parsor);
//    }
//    va_end(args);
//    std::wstring jsonStr = parsor.parse();
//    page->webview->PostWebMessageAsJson(jsonStr.data());
//}

//void MsgProcessor::emitWin(const int& eventId, int count, va_list args, JsonParsor& parsor)
//{
//    if (eventId == (int)WindowEventId::sizePosChanged) {
//        int x = va_arg(args, int);
//        int y = va_arg(args, int);
//        int w = va_arg(args, int);
//        int h = va_arg(args, int);
//        rapidjson::Value result(rapidjson::kObjectType);
//        result.AddMember("x", rapidjson::Value(x), parsor.getAllocator());
//        result.AddMember("y", rapidjson::Value(y), parsor.getAllocator());
//        result.AddMember("w", rapidjson::Value(w), parsor.getAllocator());
//        result.AddMember("h", rapidjson::Value(h), parsor.getAllocator());
//        parsor.addValue("data", std::move(result));
//    }
//    else if (eventId == (int)WindowEventId::stateChanged) {
//        int state = va_arg(args, int);
//        rapidjson::Value result(rapidjson::kObjectType);
//        result.AddMember("state", rapidjson::Value(state), parsor.getAllocator());
//        parsor.addValue("data", std::move(result));
//    }
//}

//void MsgProcessor::emitPage(const int& msgId, int count, va_list args, JsonParsor& parsor)
//{
//}
//
//void MsgProcessor::processPage(const std::string& methodName, const rapidjson::Value& params, JsonParsor& parsor)
//{
//    const rapidjson::Value::ConstArray paramsArray = params.GetArray();
//}

//void MsgProcessor::processWin(const std::string& methodName, const rapidjson::Value& params, JsonParsor& parsor)
//{
//    const rapidjson::Value::ConstArray arr = params.GetArray();
//    JsonParsor result;
//    if (methodName == "maximize") {
//        win->maximize(result);
//    }
//    else if (methodId == (int)WindowMethodId::minimize) {
//        win->minimize();
//    }
//    else if (methodId == (int)WindowMethodId::show) {
//        win->show();
//    }
//    else if (methodId == (int)WindowMethodId::hide) {
//        win->hide();
//    }
//    else if (methodId == (int)WindowMethodId::restore) {
//        win->restore();
//    }
//    else if (methodId == (int)WindowMethodId::resize) {
//        auto w = arr[0].GetInt();
//        auto h = arr[1].GetInt();
//        win->resize(w, h);
//    }
//    else if (methodId == (int)WindowMethodId::move) {
//
//    }
//    else if (methodId == (int)WindowMethodId::close) {
//        win->close();
//    }
//    else if (methodId == (int)WindowMethodId::destory) {
//        win->destroy();
//    }
//    else if (methodId == (int)WindowMethodId::flash) {
//        auto flag = arr[0].GetBool();
//        win->flash(flag);
//    }
//    else if (methodId == (int)WindowMethodId::setResizable) {
//        auto flag = arr[0].GetBool();
//        win->setResizable(flag);
//    }
//    else if (methodId == (int)WindowMethodId::startDrag) {
//        win->startDrag();
//    }
//    else if (methodId == (int)WindowMethodId::openWindow) {
//        //auto eventId = arr[0].GetObject();
//        win->openWindow();
//    }
//    else if (methodId == (int)WindowMethodId::regEvent) {
//        auto eventId = arr[0].GetInt();
//        win->regEvent(eventId);
//    }
//    else if (methodId == (int)WindowMethodId::unregEvent) {
//        auto eventId = arr[0].GetInt();
//        win->unregEvent(eventId);
//    }
//    parsor.addParsor("data", std::move(result));
//}

//void MsgProcessor::processFs(const std::string& methodName, const rapidjson::Value& params, JsonParsor& parsor)
//{
//    const rapidjson::Value::ConstArray arr = params.GetArray();
//    if (methodId == (int)FsMethodId::addResToExe) {
//        auto str1 = arr[0].GetString();
//        auto str2 = arr[1].GetString();
//        auto dirPath = Util::convertToWStr(str1);
//        auto exePath = Util::convertToWStr(str2);
//        Fs::get()->addDirAsExeRes(dirPath, exePath);
//    }
//}

//void MsgProcessor::processHorse(const std::string& methodName, const rapidjson::Value& params, JsonParsor& parsor)
//{
//    const rapidjson::Value::ConstArray arr = params.GetArray();
//    auto app = App::get();
//    if (methodId == (int)HorseMethodId::getConfig) {
//        auto str = Util::readFile(L"UI/config.json");
//        rapidjson::Document doc;
//        doc.Parse(str.data());
//        rapidjson::Value copiedValue(doc, parsor.getAllocator());
//        parsor.addValue("data", std::move(copiedValue));
//    }
//}
