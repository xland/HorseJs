#include <pch.h>

#include "MsgProcessor.h"
#include "JsonResult.h"
#include "../App/App.h"
#include "Horse.h"
#include "Fs.h"
#include "Win.h"
#include "Dialog.h"
#include "../Win/BrowserWindow.h"
#include "../Win/Page.h"

namespace {
    std::unique_ptr<MsgProcessor> msgProcessor;
    static std::unordered_map<std::string, void (Horse::*)(const rapidjson::Value&, JsonResult*)> horseFunc = {
        {"getConfig", &Horse::getConfig},
        {"createWindow", &Horse::createWindow},
    };
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
    int winId{ -1 }, tarId{-1};
    if (jsonDoc.HasMember("winId") && jsonDoc["winId"].IsInt()) {
        winId = jsonDoc["winId"].GetInt();
    }
    if (jsonDoc.HasMember("tarId") && jsonDoc["tarId"].IsInt()) {
        tarId = jsonDoc["tarId"].GetInt();
    }
    if (winId < 0 || tarId < 0) {
        MessageBox(nullptr, L"winId为空或tarId为空", L"错误", MB_OK | MB_ICONERROR);
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
    auto tar = App::get()->getWindow(tarId);
    auto result = JsonResult::create(win, tar,className, eventName);
    if (className == "horse") {
        auto it = horseFunc.find(methodName);
        if (it != horseFunc.end()) {
            (Horse::get()->*it->second)(jsonDoc["params"], result);
        }
        else {
            result->addErr(std::format("horse Method:{} not found!", methodName));
            result->returnBack();
        }
    }
    else if (className == "win") {
        auto it = winFunc.find(methodName);
        if (it != winFunc.end()) {
            (Win::get()->*it->second)(jsonDoc["params"], result);
        }
        else {
            result->addErr(std::format("win Method:{} not found!",methodName));
            result->returnBack();
        }
    }
    else if (className == "fs") {
        auto it = fsFunc.find(methodName);
        if (it != fsFunc.end()) {
            (Fs::get()->*it->second)(jsonDoc["params"], result);
        }
        else {
            result->addErr(std::format("fs Method:{} not found!",methodName));
            result->returnBack();
        }
    }
    else if (className == "dialog") {
        auto it = dialogFunc.find(methodName);
        if (it != dialogFunc.end()) {
            (Dialog::get()->*it->second)(jsonDoc["params"], result);
        }
        else {
            result->addErr(std::format("dialog Method:{} not found!",methodName));
            result->returnBack();
        }
    }
    else {
        result->addErr(std::format("className:{} not found!", className)); 
        result->returnBack();
    }
}

