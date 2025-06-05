
#include <pch.h>
#include "Process.h"

namespace {
    std::unique_ptr<Process> process;
    static std::unordered_map<std::string, void (Process::*)(const rapidjson::Value&, JsonResult*)> processFunc{
    };
}

Process::Process()
{
}

Process::~Process()
{
}

Process* Process::get()
{
    if(!process) {
        process = std::make_unique<Process>();
	}
    return process.get();
}
bool Process::excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = processFunc.find(methodName);
    if (it == processFunc.end()) return false;
    (Process::get()->*it->second)(param, result);
    return true;
}