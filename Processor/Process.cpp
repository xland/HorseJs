
#include <pch.h>
#include "Process.h"

namespace {
    std::unique_ptr<Process> process;
    static std::unordered_map<std::string, void (Process::*)(const rapidjson::Value&, JsonResult*)> funcs{
        {"exec", &Process::exec},
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
bool Process::execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Process::get()->*it->second)(param, result);
    return true;
}

void Process::exec(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring path = Util::convertToWStr(arr[0].GetString());

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    si.cb = sizeof(STARTUPINFO);
    //si.hStdError = hStdoutWrite;  // 标准错误重定向到输出管道
    //si.hStdOutput = hStdoutWrite; // 标准输出重定向到输出管道
    //si.hStdInput = hStdinRead;    // 标准输入重定向到输入管道
    //si.dwFlags |= STARTF_USESTDHANDLES; // 使用标准句柄

    if (!CreateProcess(NULL, path.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        result->addErr("can not create process");
    }
    //WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}