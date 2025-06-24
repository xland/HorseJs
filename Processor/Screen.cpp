#include <pch.h>
#include <gdiplus.h>
#include "Screen.h"
#include "../App/App.h"

namespace {
    std::unique_ptr<Screen> screen;
    static std::unordered_map<std::string, void (Screen::*)(const rapidjson::Value&, JsonResult*)> funcs{
        {"getAll", &Screen::getAll},
        {"getDesktop", &Screen::getDesktop},
        {"getColor", &Screen::getColor},
        {"getImg", &Screen::getImg},
    };
    //todo dpi变化
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
void Screen::getDesktop(const rapidjson::Value& params, JsonResult* result)
{
    auto x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    auto y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    auto w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    auto h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    result->addNumber("x", x);
    result->addNumber("y", y);
    result->addNumber("w", w);
    result->addNumber("h", h);
}
void Screen::getColor(const rapidjson::Value& params, JsonResult* result)
{
    HDC hdcScreen = GetDC(NULL);
    if (!hdcScreen) {
        result->addErr("can not get dc");
        return;
    }
    const rapidjson::Value::ConstArray arr = params.GetArray();
    POINT pt;
    if (arr.Size() == 2) {
        pt.x = arr[0].GetInt();
        pt.y = arr[1].GetInt();
    }
    else {
        GetCursorPos(&pt);
    }
    COLORREF color = GetPixel(hdcScreen, pt.x, pt.y);
    auto str = Util::colorToHex(color);
    ReleaseDC(nullptr, hdcScreen);
    result->addString("data", str.data());
}
void Screen::getImg(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    int x = arr[0].GetInt();
    int y = arr[1].GetInt();
    int w = arr[2].GetInt();
    int h = arr[3].GetInt();

    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    HGDIOBJ oldObj = SelectObject(hDC, hBitmap);
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    SelectObject(hDC, oldObj);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    std::vector<std::byte> pngData;
    Util::bitmapToPngData(hBitmap, pngData,w,h);
    DeleteObject(hBitmap);

    auto env12 = App::get()->env.try_query<ICoreWebView2Environment12>();
    wil::com_ptr<ICoreWebView2SharedBuffer> sharedBuffer;
    auto hr = env12->CreateSharedBuffer(pngData.size(), &sharedBuffer);
    if (FAILED(hr)) {
        result->addErr("CreateSharedBuffer err");
        return;
    }
    wil::com_ptr<IStream> stream;
    sharedBuffer->OpenStream(&stream);
    stream->Write(pngData.data(), pngData.size(), nullptr);
    result->addNumber("totalSize", (long long)pngData.size());
    result->returnBackSharedBuffer(sharedBuffer.get());
    sharedBuffer->Close();
    result->cancel = true;
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