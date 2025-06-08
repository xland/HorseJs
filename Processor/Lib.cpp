#include <pch.h>
#include "Lib.h"

namespace {
    std::unique_ptr<Lib> lib;
    static std::unordered_map<std::string, void (Lib::*)(const rapidjson::Value&, JsonResult*)> funcs{
    {"load", &Lib::load},
    {"free", &Lib::free},
    {"call", &Lib::call},
    };
    std::unordered_map<int,HMODULE> dllModules;
    typedef const char* (*FuncPtr)(const char*);
}

Lib::Lib()
{
}

Lib::~Lib()
{
}

Lib* Lib::get()
{
    //todo 这个类  可能会涉及到  sharedBuffer
    //todo 可能还涉及到事件
    if(!lib) {
        lib = std::make_unique<Lib>();
	}
    return lib.get();
}
bool Lib::execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Lib::get()->*it->second)(param, result);
    return true;
}
void Lib::load(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring dllPath = Util::convertToWStr(arr[0].GetString());
    HMODULE hDll = LoadLibrary(dllPath.data());
    if (hDll == NULL) {
        result->addErr("load dll error.");
        return;
    }
    static int id{ 0 };
    dllModules.insert({ id,hDll });
    id += 1;
    result->addNumber("id", id);
}

void Lib::free(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto id = arr[0].GetInt();
    auto flag = FreeLibrary(dllModules[id]);
    if (!flag) {
        result->addErr("free dll error.");
        return;
    }
    dllModules.erase(id);
}

void Lib::call(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto id = arr[0].GetInt();
    auto funcName = arr[1].GetString();
    auto funcParam = arr[2].GetString();
    FuncPtr func = reinterpret_cast<FuncPtr>(GetProcAddress(dllModules[id], funcName));
    if (func == NULL) {
        result->addErr("failed to get function address.");
        return;
    }
    auto data = func(funcParam);
    result->addString("data", data);
}
