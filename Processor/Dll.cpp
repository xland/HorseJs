#include <pch.h>
#include "Dll.h"

namespace {
    std::unique_ptr<Dll> dll;
    static std::unordered_map<std::string, void (Dll::*)(const rapidjson::Value&, JsonResult*)> funcs{
    {"load", &Dll::load},
    {"free", &Dll::free},
    {"invoke", &Dll::invoke},
    };
    std::unordered_map<int,HMODULE> dllModules;
    typedef const char* (__cdecl *FuncPtr)(const char*);
}

Dll::Dll()
{
}

Dll::~Dll()
{
}

Dll* Dll::get()
{
    //todo 这个类  可能会涉及到  sharedBuffer
    //todo 可能还涉及到事件
    if(!dll) {
        dll = std::make_unique<Dll>();
	}
    return dll.get();
}
bool Dll::execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Dll::get()->*it->second)(param, result);
    return true;
}
void Dll::load(const rapidjson::Value& params, JsonResult* result)
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

void Dll::free(const rapidjson::Value& params, JsonResult* result)
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

void Dll::invoke(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto id = arr[0].GetInt();
    std::string funcName = arr[1].GetString();
    std::string funcParam = arr[2].GetString();

    HMODULE hDll = LoadLibrary(L"TestDll.dll");
    if (hDll == NULL) {
        result->addErr("load dll error.");
        return;
    }

    FuncPtr func = reinterpret_cast<FuncPtr>(GetProcAddress(hDll, funcName.data()));
    if (func == NULL) {
        result->addErr("failed to get function address.");
        return;
    }
    auto data = func(funcParam.data());
    result->addString("data", data);
}
