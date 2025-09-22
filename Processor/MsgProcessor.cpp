#include <pch.h>
#include "../App/App.h"
#include "../App/BrowserWindow.h"
#include "MsgProcessor.h"
#include "JsonResult.h"
#include "Clipboard.h"
#include "Dialog.h"
#include "Horse.h"
#include "Dll.h"
#include "Fs.h"
#include "Win.h"
#include "Os.h"
#include "Screen.h"

namespace {
    std::unique_ptr<MsgProcessor> msgProcessor;

    static std::unordered_map<std::string, std::function<bool(std::string&, const rapidjson::Value&, JsonResult*)>> processFunc{
        {"clipboard",Clipboard::execute},
        {"dialog",Dialog::execute},
        {"dll",Dll::execute},
        {"fs",Fs::execute},
        {"horse",Horse::execute},
        {"os",Os::execute},
        {"screen",Screen::execute},
        {"win",Win::execute},
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
    JsonResult result(winId,className,eventName);
    auto it = processFunc.find(className);
    if (it == processFunc.end()) {
        result.addErr(std::format("class {} not found!", className));
    }
    else {
        auto flag = (*it).second(methodName, jsonDoc["params"], &result);
        if (!flag) {
            result.addErr(std::format("method {} not found!", methodName));
        }
    }
    result.returnBack();
}

