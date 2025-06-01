#include <pch.h>

#include "MsgProcessor.h"
#include "JsonResult.h"
#include "App.h"
#include "Fs.h"
#include "Win.h"
#include "Dialog.h"
#include "../Win/BrowserWindow.h"
#include "../Win/Page.h"

namespace {
    std::unique_ptr<MsgProcessor> msgProcessor;
    static std::unordered_map<std::string, void (Win::*)(const rapidjson::Value&, JsonResult*)> winFunc = {
        {"show", &Win::show},
        {"hide", &Win::hide},
        {"maximize", &Win::maximize},
        {"minimize", &Win::minimize},
        {"restore", &Win::restore},
        {"close", &Win::close},
        {"destroy", &Win::destroy},
        {"startDrag", &Win::startDrag},
        {"openWindow", &Win::openWindow},
        {"setResizable", &Win::setResizable},
        {"resize", &Win::resize},
        {"addEventListener", &Win::addEventListener},
        {"removeEventListener", &Win::removeEventListener},
    };
    static std::unordered_map<std::string, void (Fs::*)(const rapidjson::Value&, JsonResult*)> fsFunc = {
        {"getFileInfo", &Fs::getFileInfo},
        {"exists", &Fs::exists},
        {"readFile", &Fs::readFile},
        {"readFileChunk",&Fs::readFileChunk},
        {"writeFile", &Fs::writeFile},
        {"writeFileChunk", &Fs::writeFileChunk},
        {"delPath", &Fs::delPath},
        {"removePath", &Fs::removePath},
        {"createDir", &Fs::createDir},
        {"listDir", &Fs::listDir},
        {"copyFile", &Fs::copyFile},
        {"moveFile", &Fs::moveFile},
        {"renameFile", &Fs::renameFile},
        {"watch", &Fs::watch},
    };
    static std::unordered_map<std::string, void (Dialog::*)(const rapidjson::Value&, JsonResult*)> dialogFunc = {
        {"openPathDialog", &Dialog::openPathDialog},
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
    int winId{ -1 };
    if (jsonDoc.HasMember("winId") && jsonDoc["winId"].IsInt()) {
        winId = jsonDoc["winId"].GetInt();
    }
    if (winId < 0) {
        MessageBox(nullptr, L"winId为空", L"错误", MB_OK | MB_ICONERROR);
        return;
    }
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
    if (className.empty() || methodName.empty() || eventName.empty()) {
        MessageBox(nullptr, L"className,methodName或eventName", L"错误", MB_OK | MB_ICONERROR);
        return;
    }
    auto win = App::get()->getWindow(winId);
    auto result = JsonResult::create(win, className, eventName);
    if (className == "win") {
        auto it = winFunc.find(methodName);
        if (it != winFunc.end()) {
            (Win::get()->*it->second)(jsonDoc["params"], result);
        }
        else {
            result->addString("err", std::format("Window Method:{} not found!",methodName));
            result->returnBack();
        }
    }
    else if (className == "fs") {
        auto it = fsFunc.find(methodName);
        if (it != fsFunc.end()) {
            (Fs::get()->*it->second)(jsonDoc["params"], result);
        }
        else {
            result->addString("err", std::format("Fs Method:{} not found!",methodName));
            result->returnBack();
        }
    }
    else if (className == "dialog") {
        auto it = dialogFunc.find(methodName);
        if (it != dialogFunc.end()) {
            (Dialog::get()->*it->second)(jsonDoc["params"], result);
        }
        else {
            result->addString("err", std::format("Dialog Method:{} not found!",methodName));
            result->returnBack();
        }
    }
    else {
        result->addString("err", std::format("className:{} not found!", className)); 
        result->returnBack();
    }
}

