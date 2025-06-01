#include "Win.h"
#include "App.h"
#include "../Win/BrowserWindow.h"
#include "../Win/BrowserWindowConfig.h"
#include "../Win/Page.h"
namespace {
    std::unique_ptr<Win> fs;
}

Win::Win()
{
}

Win::~Win()
{
}

Win* Win::get()
{
    if (!fs) {
        fs = std::make_unique<Win>();
    }
    return fs.get();
}

void Win::addEventListener(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto eventName = arr[0].GetString();
    if (!win->eventFlag.contains(eventName)) {
        result.addString("err", "doesn't have this event.");
    }
    win->eventFlag[eventName] = true;
}

void Win::removeEventListener(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto eventName = arr[0].GetString();
    if (!win->eventFlag.contains(eventName)) {
        result.addString("err", "doesn't have this event.");
    }
    win->eventFlag[eventName] = false;
}

void Win::maximize(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result)
{
    if (!win->config->maximizable) {
        result.addString("err", "failed due to the maximizable or maxSize settings in config.json.");
        return;
    }
    ShowWindow(win->hwnd, SW_MAXIMIZE);
}

void Win::minimize(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result)
{
    if (win->ctrlComp) {
        //由于窗口最小化了，WebView2 内部的 Chromium 引擎判定视图不可见，不再处理鼠标事件。
        //todo 还得拦截这个消息： case WM_SYSCOMMAND: switch (wParam & 0xFFF0) { case SC_MINIMIZE:
        win->ctrlComp->SendMouseInput(COREWEBVIEW2_MOUSE_EVENT_KIND_LEAVE,
            COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_NONE, 0, POINT{});
    }
    ShowWindow(win->hwnd, SW_MINIMIZE);
}
void Win::show(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result)
{
    ShowWindow(win->hwnd, SW_SHOW);
    SetForegroundWindow(win->hwnd);
    //ShowWindow(hwnd, SW_SHOWNORMAL);
}

void Win::hide(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result)
{
    ShowWindow(win->hwnd, SW_HIDE);
}

void Win::restore(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result)
{
    ShowWindow(win->hwnd, SW_RESTORE);
}

void Win::close(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result)
{
    //CloseWindow(hwnd); 这相当于窗口最小化
    //不能用SendMessage，因为这回导致对象删除之后，MsgProcessor还在准备向页面发消息
    PostMessage(win->hwnd, WM_CLOSE, 0, 0);
}

void Win::destroy(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result)
{
    PostMessage(win->hwnd, WM_DESTROY, 0, 0);
}

void Win::flash(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result)
{
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

void Win::setResizable(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto flag = arr[0].GetBool();
    if (!win->config->frame) {
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

void Win::startDrag(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result)
{
    ReleaseCapture();
    SendMessage(win->hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

void Win::openWindow(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    const rapidjson::Value& value = arr[0];
    auto winIns = std::make_unique<BrowserWindow>(value);
    if (value.HasMember("page") && value["page"].IsObject()) {
        const rapidjson::Value& pageObject = value["page"];
        win->load(pageObject);
    }
    App::get()->winMap.insert({ win->config->id,std::move(winIns) });
}

void Win::resize(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto w = arr[0].GetInt();
    auto h = arr[0].GetInt();
    SetWindowPos(win->hwnd, nullptr, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);
}
