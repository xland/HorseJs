#include <pch.h>
#include "Screen.h"

namespace {
    std::unique_ptr<Screen> screen;
    static std::unordered_map<std::string, void (Screen::*)(const rapidjson::Value&, JsonResult*)> funcs{
        {"getAll", &Screen::getAll},
    };
    //todo dpi变化
    //todo 指定点的颜色信息（窗口位置、屏幕位置）
    using Context = std::pair<rapidjson::Value&, rapidjson::Document::AllocatorType&>;
}

Screen::Screen()
{
}

Screen::~Screen()
{
}

Screen* Screen::get()
{
    if(!screen) {
        screen = std::make_unique<Screen>();
	}
    return screen.get();
}
bool Screen::execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Screen::get()->*it->second)(param, result);
    return true;
}
void Screen::getAll(const rapidjson::Value& params, JsonResult* result)
{
    rapidjson::Value array(rapidjson::kArrayType);
    auto& allocator = result->getAllocator();
    Context context(array, allocator);
    Context* ptr = &context;
    EnumDisplayMonitors(NULL, NULL, Screen::enumProc, (LPARAM)ptr);
    result->addValue("data", std::move(array));
}
BOOL Screen::enumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    if (GetMonitorInfo(hMonitor, &monitorInfo)) {

        auto context = (Context*)dwData;
        auto& arr = context->first;
        auto& allocator = context->second;
        rapidjson::Value obj(rapidjson::kObjectType);
        {
            rapidjson::Value key("device", allocator);
            auto val = std::wstring(monitorInfo.szDevice);
            auto valStr = Util::convertToStr(val);
            rapidjson::Value value(valStr.data(), allocator);
            obj.AddMember(key, value, allocator);
        }
        {
            rapidjson::Value key("right1", allocator);
            obj.AddMember(key, (unsigned)monitorInfo.rcMonitor.right, allocator);
        }
        {
            rapidjson::Value key("bottom1", allocator);
            obj.AddMember(key, (unsigned)monitorInfo.rcMonitor.bottom, allocator);
        }
        {
            rapidjson::Value key("left1", allocator);
            obj.AddMember(key, (unsigned)monitorInfo.rcMonitor.left, allocator);
        }
        {
            rapidjson::Value key("top1", allocator);
            obj.AddMember(key, (unsigned)monitorInfo.rcMonitor.top, allocator);
        }
        {
            rapidjson::Value key("right2", allocator);
            obj.AddMember(key, (unsigned)monitorInfo.rcWork.right, allocator);
        }
        {
            rapidjson::Value key("bottom2", allocator);
            obj.AddMember(key, (unsigned)monitorInfo.rcWork.bottom, allocator);
        }
        {
            rapidjson::Value key("left2", allocator);
            obj.AddMember(key, (unsigned)monitorInfo.rcWork.left, allocator);
        }
        {
            rapidjson::Value key("top2", allocator);
            obj.AddMember(key, (unsigned)monitorInfo.rcWork.top, allocator);
        }
        {
            rapidjson::Value key("isMain", allocator);
            obj.AddMember(key, (bool)(monitorInfo.dwFlags & MONITORINFOF_PRIMARY), allocator);
        }
        {
            UINT dpiX, dpiY;
            HRESULT hr = GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
            float scaleFactor = dpiX / 96.0f * 100;
            rapidjson::Value key("scaleFactor", allocator);
            obj.AddMember(key, (unsigned)scaleFactor, allocator);
        }
        arr.PushBack(obj, allocator);
    }
    return TRUE;
}
//获取屏幕上某个点的颜色
//获取分辨率和DPI
//