#include <pch.h>
#include "../App/App.h"
#include "../App/BrowserWindow.h"
#include "MsgProcessor.h"
#include "JsonResult.h"
#include "Clipboard.h"
#include "Dialog.h"
#include "Horse.h"
#include "Lib.h"
#include "Net.h"
#include "Fs.h"
#include "Notify.h"
#include "Win.h"
#include "Os.h"
#include "Screen.h"
#include "Tray.h"
#include "Process.h"

namespace {
    std::unique_ptr<MsgProcessor> msgProcessor;

    static std::unordered_map<std::string, std::function<bool(std::string&, const rapidjson::Value&, JsonResult*)>> processFunc{
        {"clipboard",Clipboard::excute},
        {"dialog",Dialog::excute},
        {"horse",Horse::excute},
        {"lib",Lib::excute},
        {"net",Net::excute},
        {"fs",Fs::excute},
        {"notify",Notify::excute},
        {"win",Win::excute},
        {"os",Os::excute},
        {"screen",Screen::excute},
        {"tray",Tray::excute},
        {"process",Process::excute},
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
    JsonResult result(winId,className, eventName);
    if (jsonDoc.HasMember("tarId") && jsonDoc["tarId"].IsInt()) {
        result.tarId = jsonDoc["tarId"].GetInt();
    }
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

