#include <memory>

#include "MsgProcessor.h"
#include "App.h"
#include "Fs.h"
#include "JsonParsor.h"
#include "../Win/BrowserWindow.h"
#include "../Win/Page.h"
#include "Fs.h"

namespace {
    std::unique_ptr<MsgProcessor> msgProcessor;
    static std::unordered_map<std::string, void (BrowserWindow::*)(const rapidjson::Value&, JsonParsor&)> winFunc = {
        {"show", &BrowserWindow::show},
        {"hide", &BrowserWindow::hide},
        {"maximize", &BrowserWindow::maximize},
        {"minimize", &BrowserWindow::minimize},
        {"restore", &BrowserWindow::restore},
        {"close", &BrowserWindow::close},
        {"destroy", &BrowserWindow::destroy},
        {"startDrag", &BrowserWindow::startDrag},
        {"openWindow", &BrowserWindow::openWindow},
        {"setResizable", &BrowserWindow::setResizable},
        {"resize", &BrowserWindow::resize},
        {"addEventListener", &BrowserWindow::addEventListener},
        {"removeEventListener", &BrowserWindow::removeEventListener},
    };
    static std::unordered_map<std::string, void (Fs::*)(BrowserWindow*,const rapidjson::Value&, JsonParsor&)> fsFunc = {
        {"stat", &Fs::stat},
        {"exists", &Fs::exists},
        {"readFile", &Fs::readFile},
        {"writeFile", &Fs::writeFile},
        {"removeFile", &Fs::removeFile},
        {"removeDir", &Fs::removeDir},
        {"createDir", &Fs::createDir},
        {"listDir", &Fs::listDir},
        {"copyFile", &Fs::copyFile},
        {"moveFile", &Fs::moveFile},
        {"renameFile", &Fs::renameFile},
        {"watch", &Fs::watch},
    };
    static std::unordered_map<std::string, void (BrowserWindow::*)(const rapidjson::Value&, JsonParsor&)> osFunc = {
        {"maximize", &BrowserWindow::maximize},
    };
}

MsgProcessor* MsgProcessor::get() {
    if (!msgProcessor) {
        msgProcessor = std::make_unique<MsgProcessor>();
    }
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
    int winId{ -1 };
    if (jsonDoc.HasMember("winId") && jsonDoc["winId"].IsInt()) {
        winId = jsonDoc["winId"].GetInt();
    }
    if (className.empty() || methodName.empty() || eventName.empty()) {
        MessageBox(nullptr, L"className,methodName,eventName,tarId,srcId为空", L"错误", MB_OK | MB_ICONERROR);
        return;
    }
    auto win = App::get()->getWindow(winId);
    JsonParsor parsor;
    parsor.addString("className", className);
    parsor.addString("eventName", eventName);
    if (className == "horse") {
        //processHorse(methodName, jsonDoc["params"], parsor);
    }
    else if (className == "window") {
        auto it = winFunc.find(methodName);
        if (it != winFunc.end()) {
            (win->*it->second)(jsonDoc["params"], parsor);
        }
        else {
            parsor.addString("err", "Window Method not found!");
        }
    }
    else if (className == "page") {
        //processPage(methodName, jsonDoc["params"],parsor);
    }
    else if (className == "fs") {
        auto it = fsFunc.find(methodName);
        if (it != fsFunc.end()) {
            (Fs::get()->*it->second)(win,jsonDoc["params"], parsor);
        }
        else {
            parsor.addString("err", "Fs Method not found!");
        }
    }
    //返回值，必须放在后面，因为win有可能被销毁
    std::wstring jsonStr = parsor.parse();
    win->page->webview->PostWebMessageAsJson(jsonStr.data());
}

