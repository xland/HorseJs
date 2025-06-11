#include <pch.h>
#include "Horse.h"
#include "../App/App.h"
#include "../App/BrowserWindow.h"
#include "../Res/Res.h"

namespace {
    std::unique_ptr<Horse> horse;
    static std::unordered_map<std::string, void (Horse::*)(const rapidjson::Value&, JsonResult*)> funcs{
    {"getConfig", &Horse::getConfig},
    {"createWindow", &Horse::createWindow},
    {"getVersion", &Horse::getVersion},
    {"quit", &Horse::quit},
    {"exit", &Horse::exit},
    };
}

Horse::Horse()
{
}

Horse::~Horse()
{
}

Horse* Horse::get()
{
    if(!horse) {
        horse = std::make_unique<Horse>();
	}
    return horse.get();
}
bool Horse::execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Horse::get()->*it->second)(param, result);
    return true;
}
void Horse::getConfig(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto str = Util::readFile(L"UI/config.json");
    rapidjson::Document doc;
    doc.Parse(str.data());
    rapidjson::Value copiedValue(doc, result->getAllocator());
	result->addValue("data", std::move(copiedValue));
}

void Horse::createWindow(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    const rapidjson::Value& value = arr[0];
    auto winIns = std::make_unique<BrowserWindow>(value);
    winIns->load();
    result->addNumber("id", winIns->id);
    App::addWindow(std::move(winIns));
}

void Horse::getVersion(const rapidjson::Value& params, JsonResult* result) 
{
    std::vector<wchar_t> exePath(MAX_PATH);
    if (GetModuleFileName(nullptr, exePath.data(), static_cast<DWORD>(exePath.size())) == 0) {
        result->addErr("Failed to get executable path");
    }
    // 获取版本信息大小
    DWORD dummy;
    DWORD versionSize = GetFileVersionInfoSize(exePath.data(), &dummy);
    if (versionSize == 0) {
        result->addErr("Failed to get version info size");
    }
    // 分配缓冲区并获取版本信息
    std::vector<BYTE> versionData(versionSize);
    if (!GetFileVersionInfoW(exePath.data(), 0, versionSize, versionData.data())) {
        result->addErr("Failed to get version info");
    }
    // 查询固定版本信息（VS_FIXEDFILEINFO）
    VS_FIXEDFILEINFO* fileInfo = nullptr;
    UINT fileInfoSize = 0;
    if (!VerQueryValueW(versionData.data(), L"\\", reinterpret_cast<void**>(&fileInfo), &fileInfoSize)) {
        result->addErr("Failed to query version info");
    }
    // 提取版本号
    DWORD major = (fileInfo->dwFileVersionMS >> 16) & 0xFFFF;
    DWORD minor = fileInfo->dwFileVersionMS & 0xFFFF;
    DWORD patch = (fileInfo->dwFileVersionLS >> 16) & 0xFFFF;
    DWORD build = fileInfo->dwFileVersionLS & 0xFFFF;
    auto& allocator = result->getAllocator();
    rapidjson::Value array1(rapidjson::kArrayType);
    array1.PushBack(VERSION_MAJOR, allocator);
    array1.PushBack(VERSION_MINOR, allocator);
    array1.PushBack(VERSION_PATCH, allocator);
    array1.PushBack(VERSION_BUILD, allocator);
    result->addValue("exeVer", std::move(array1));
    rapidjson::Value array2(rapidjson::kArrayType);
    array2.PushBack(VERSION_MAJOR, allocator);
    array2.PushBack(VERSION_MINOR, allocator);
    array2.PushBack(VERSION_PATCH, allocator);
    array2.PushBack(VERSION_BUILD, allocator);
    result->addValue("horseVer", std::move(array2));
}

void Horse::quit(const rapidjson::Value& params, JsonResult* result)
{
    App::closeAllWindowAsync();
    for (size_t i = 0; i < 8; i++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (!App::hasWindow()) {
            PostQuitMessage(0);
            break;
        }
    }
    result->addErr("can not close all win");
}

void Horse::exit(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    int val = 0;
    if (!arr[0].IsNull() && arr[0].IsInt()) {
        auto val = arr[0].GetInt();
    }
    PostQuitMessage(val);
}
