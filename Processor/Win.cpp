#include <pch.h>
#include "Win.h"
#include "../App/App.h"
#include "../App/BrowserWindow.h"
namespace {
    std::unique_ptr<Win> win;
    static std::unordered_map<std::string, void (Win::*)(const rapidjson::Value&, JsonResult*)> funcs{
    {"show", &Win::show},
    {"hide", &Win::hide},
    {"maximize", &Win::maximize},
    {"minimize", &Win::minimize},
    {"restore", &Win::restore},
    {"close", &Win::close},
    {"destroy", &Win::destroy},
    {"startDrag", &Win::startDrag},
    {"setResizable", &Win::setResizable},
    {"resize", &Win::resize},
    {"flash", &Win::flash},
    {"addEventListener", &Win::addEventListener},
    {"removeEventListener", &Win::removeEventListener},
    };
}

Win::Win()
{
}

Win::~Win()
{
}

Win* Win::get()
{
    if (!win) {
        win = std::make_unique<Win>();
    }
    return win.get();
}
bool Win::execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Win::get()->*it->second)(param, result);
    return true;
}
void Win::addEventListener(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::string eventName = arr[0].GetString();
    auto tar = result->getTar();
    tar->events[eventName].insert(result->winId);
}

void Win::removeEventListener(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::string eventName = arr[0].GetString();
    auto tar = result->getTar();
    tar->events[eventName].erase(result->winId);
}

void Win::maximize(const rapidjson::Value& params, JsonResult* result)
{
    auto win = result->getTar();
    if (!win->maximizable) {
        result->addErr("failed due to the maximizable or maxSize settings in config.json.");
        return;
    }
    ShowWindow(win->hwnd, SW_MAXIMIZE);
}

void Win::minimize(const rapidjson::Value& params, JsonResult* result)
{
    auto win = result->getTar();
    if (win->ctrlComp) {
        //由于窗口最小化了，WebView2 内部的 Chromium 引擎判定视图不可见，不再处理鼠标事件。
        //todo 还得拦截这个消息： case WM_SYSCOMMAND: switch (wParam & 0xFFF0) { case SC_MINIMIZE:
        win->ctrlComp->SendMouseInput(COREWEBVIEW2_MOUSE_EVENT_KIND_LEAVE,
            COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_NONE, 0, POINT{});
    }
    ShowWindow(win->hwnd, SW_MINIMIZE);
}
void Win::show(const rapidjson::Value& params, JsonResult* result)
{
    auto win = result->getTar();
    ShowWindow(win->hwnd, SW_SHOW);
    SetForegroundWindow(win->hwnd);
    //ShowWindow(hwnd, SW_SHOWNORMAL);
}

void Win::hide(const rapidjson::Value& params, JsonResult* result)
{
    auto win = result->getTar();
    ShowWindow(win->hwnd, SW_HIDE);
}

void Win::restore(const rapidjson::Value& params, JsonResult* result)
{
    auto win = result->getTar();
    ShowWindow(win->hwnd, SW_RESTORE);
}

void Win::close(const rapidjson::Value& params, JsonResult* result)
{
    auto win = result->getTar();
    //CloseWindow(hwnd); 这相当于窗口最小化
    //不能用SendMessage，因为这回导致对象删除之后，MsgProcessor还在准备向页面发消息
    PostMessage(win->hwnd, WM_CLOSE, 0, 0);
}

void Win::destroy(const rapidjson::Value& params, JsonResult* result)
{
    auto win = result->getTar();
    PostMessage(win->hwnd, WM_DESTROY, 0, 0);
}

void Win::flash(const rapidjson::Value& params, JsonResult* result)
{
    auto win = result->getTar();
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto isStart = arr[0].GetBool();
    FLASHWINFO fwInfo = {};
    fwInfo.cbSize = sizeof(FLASHWINFO);
    fwInfo.hwnd = win->hwnd;
    if (isStart) {
        fwInfo.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
    }
    else
    {
        fwInfo.dwFlags = FLASHW_STOP;
    }
    fwInfo.uCount = 0;                   // uCount = 0 且设置了 FLASHW_TIMERNOFG 时，会持续闪烁直到窗口变为前台；
    fwInfo.dwTimeout = 0;                 // 使用默认闪烁时间
    FlashWindowEx(&fwInfo);
}

void Win::setResizable(const rapidjson::Value& params, JsonResult* result)
{
    auto win = result->getTar();
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto flag = arr[0].GetBool();
    if (!win->frame) {
        win->framelessResizable = flag;
        return;
    }
    LONG style = GetWindowLong(win->hwnd, GWL_STYLE);
    if (flag) {
        style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }
    else {
        style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    }
    SetWindowLong(win->hwnd, GWL_STYLE, style);
    SetWindowPos(win->hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
}

void Win::startDrag(const rapidjson::Value& params, JsonResult* result)
{
    auto win = result->getTar();
    ReleaseCapture();
    SendMessage(win->hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

void Win::resize(const rapidjson::Value& params, JsonResult* result)
{
    auto win = result->getTar();
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto w = arr[0].GetInt();
    auto h = arr[0].GetInt();
    SetWindowPos(win->hwnd, nullptr, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);
}

void Win::insertMenu(const rapidjson::Value& params, JsonResult* result)
{
}

void Win::removeMenu(const rapidjson::Value& params, JsonResult* result)
{
}
