#include <windowsx.h>
#include <dwmapi.h>
#include <winrt/base.h>

#include "../App/App.h"
#include "../App/MsgProcessor.h"
#include "BrowserWindow.h"
#include "BrowserWindowConfig.h"
#include "Page.h"

BrowserWindow::BrowserWindow(const rapidjson::Value& winConfig)
    : config{std::make_unique<BrowserWindowConfig>(winConfig)},
    eventFlag{
        {"closing",false},
        {"sizePosChanged",false},
        {"stateChanged",false},
    }
{
    initWindow();
}

BrowserWindow::~BrowserWindow()
{
}

#pragma region JsMethod

void BrowserWindow::addEventListener(const rapidjson::Value& params, JsonParsor& result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto eventName = arr[0].GetString();
    if (!eventFlag.contains(eventName)) {
        result.addString("err", "doesn't have this event.");
    }
    eventFlag[eventName] = true;
}

void BrowserWindow::removeEventListener(const rapidjson::Value& params, JsonParsor& result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto eventName = arr[0].GetString();
    if (!eventFlag.contains(eventName)) {
        result.addString("err", "doesn't have this event.");
    }
    eventFlag[eventName] = false;
}

void BrowserWindow::maximize(const rapidjson::Value& params, JsonParsor& result)
{
    if (!config->maximizable) {
        result.addString("err", "failed due to the maximizable or maxSize settings in config.json.");
        return;
    }
    ShowWindow(hwnd, SW_MAXIMIZE);
}

void BrowserWindow::minimize(const rapidjson::Value& params, JsonParsor& result)
{
    if (ctrlComp){
        //由于窗口最小化了，WebView2 内部的 Chromium 引擎判定视图不可见，不再处理鼠标事件。
        //todo 还得拦截这个消息： case WM_SYSCOMMAND: switch (wParam & 0xFFF0) { case SC_MINIMIZE:
        ctrlComp->SendMouseInput(COREWEBVIEW2_MOUSE_EVENT_KIND_LEAVE,
            COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_NONE, 0, POINT{});
    }
    ShowWindow(hwnd, SW_MINIMIZE);
}
void BrowserWindow::show(const rapidjson::Value& params, JsonParsor& result)
{
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    //ShowWindow(hwnd, SW_SHOWNORMAL);
}

void BrowserWindow::hide(const rapidjson::Value& params, JsonParsor& result)
{
    ShowWindow(hwnd, SW_HIDE);
}

void BrowserWindow::restore(const rapidjson::Value& params, JsonParsor& result)
{
    ShowWindow(hwnd, SW_RESTORE);
}

void BrowserWindow::close(const rapidjson::Value& params, JsonParsor& result)
{
    //CloseWindow(hwnd); 这相当于窗口最小化
    //不能用SendMessage，因为这回导致对象删除之后，MsgProcessor还在准备向页面发消息
    PostMessage(hwnd, WM_CLOSE, 0, 0);
}

void BrowserWindow::destroy(const rapidjson::Value& params, JsonParsor& result)
{
    PostMessage(hwnd, WM_DESTROY, 0, 0);
}

void BrowserWindow::flash(const rapidjson::Value& params, JsonParsor& result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto isStart = arr[0].GetBool();
    FLASHWINFO fwInfo = {};
    fwInfo.cbSize = sizeof(FLASHWINFO);
    fwInfo.hwnd = hwnd;
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

void BrowserWindow::setResizable(const rapidjson::Value& params, JsonParsor& result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto flag = arr[0].GetBool();
    if (!config->frame) {
        framelessResizable = flag;
        return;
    }
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    if (flag) {
        style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }
    else {
        style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
    }
    SetWindowLong(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
}

void BrowserWindow::startDrag(const rapidjson::Value& params, JsonParsor& result)
{
    ReleaseCapture();
    SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}

void BrowserWindow::openWindow(const rapidjson::Value& params, JsonParsor& result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
	const rapidjson::Value& value = arr[0];
    auto win = std::make_unique<BrowserWindow>(value);
    if (value.HasMember("page") && value["page"].IsObject()) {
        const rapidjson::Value& pageObject = value["page"];
        win->load(pageObject);
    }
    App::get()->winMap.insert({ win->config->id,std::move(win) });
}

void BrowserWindow::resize(const rapidjson::Value& params, JsonParsor& result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto w = arr[0].GetInt();
    auto h = arr[0].GetInt();
    SetWindowPos(hwnd, nullptr, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);
}
#pragma endregion

void BrowserWindow::initWindow()
{
    long exStyle, style;
    setWindowStyle(exStyle, style);
    hwnd = CreateWindowEx(exStyle, getWinClsName().data(), config->title.data(), style,
        config->x, config->y, config->w, config->h, nullptr, nullptr, App::get()->hInstance, nullptr);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    if (!config->frame && config->shadow)
    {
        MARGINS margins = { 1, 1, 1, 1 };
        DwmExtendFrameIntoClientArea(hwnd, &margins);
        int value = 2;
        DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &value, sizeof(value));
        DwmSetWindowAttribute(hwnd, DWMWA_ALLOW_NCPAINT, &value, sizeof(value));
    }

}
LRESULT BrowserWindow::winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST)
    {
        routeMsgToPage(msg, wParam, lParam);
        return true;
    }
    else if (msg == WM_NCHITTEST) {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        ScreenToClient(hwnd, &pt);
        return hittest(pt);
    }
    else if (msg == WM_MOUSELEAVE)
    {
        isMouseTracking = false;
        if (!ctrlComp) goto sysProcess;
        ctrlComp->SendMouseInput(COREWEBVIEW2_MOUSE_EVENT_KIND_LEAVE,
            COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_NONE, 0, POINT{});
        return true;
    }
    else if(msg == WM_SETCURSOR){
        if (LOWORD(lParam) != HTCLIENT) goto sysProcess;
        if (!ctrlComp) goto sysProcess;
        HCURSOR cursor = nullptr;
        auto hr = ctrlComp->get_Cursor(&cursor);
        if (FAILED(hr)) return false;
        if(!cursor) goto sysProcess;
        SetCursor(cursor);
        return true;
    }
    else if (msg == WM_CLOSE) {
        closing();
        return false;
    }
    else if (msg == WM_DESTROY) {
        SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
        App::get()->onWindowDestroy(this);
        return false;
    }
    else if (msg == WM_GETMINMAXINFO) {
        MINMAXINFO* mmi = (PMINMAXINFO)lParam;
        setMinMaxInfo(mmi);
        return false;
    }
    else if (msg == WM_SIZE) {
        stateChanged(wParam);
    }
    else if (msg == WM_WINDOWPOSCHANGED) {
        WINDOWPOS* winPos = reinterpret_cast<WINDOWPOS*>(lParam);
        sizePosChanged(winPos);
    }
    sysProcess:
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void BrowserWindow::sizePosChanged(WINDOWPOS* winPos)
{
    config->x = winPos->x;
    config->y = winPos->y;
    config->w = winPos->cx;
    config->h = winPos->cy;
    if (!ctrl) return;
    RECT bounds;
    GetClientRect(hwnd, &bounds);
    ctrl->put_Bounds(bounds);
    if (!eventFlag["sizePosChanged"]) return;
    JsonParsor eventData;
    eventData.addString("className", "window");
    eventData.addString("eventName", "sizePosChanged");
    eventData.addNumber("x", config->x);
    eventData.addNumber("y", config->y);
    eventData.addNumber("w", config->w);
    eventData.addNumber("h", config->h);
    std::wstring jsonStr = eventData.parse();
    page->webview->PostWebMessageAsJson(jsonStr.data());
}
void BrowserWindow::stateChanged(const int& state)
{
    if (!ctrl || !eventFlag["stateChanged"]) return;
    JsonParsor eventData;
    eventData.addString("className", "window");
    eventData.addString("eventName", "stateChanged");
    if (state == SIZE_MAXIMIZED) {
        eventData.addString("state", "maximize");
    }
    else if (state == SIZE_MINIMIZED) {
        eventData.addString("state", "minimize");
    }
    else if (state == SIZE_RESTORED) {
        eventData.addString("state", "restore");
    }
    std::wstring jsonStr = eventData.parse();
    page->webview->PostWebMessageAsJson(jsonStr.data());
}
void BrowserWindow::closing()
{
    if (eventFlag["closing"]) {
        JsonParsor eventData;
        eventData.addString("className", "window");
        eventData.addString("eventName", "closing");
        std::wstring jsonStr = eventData.parse();
        page->webview->PostWebMessageAsJson(jsonStr.data());
        return; //阻止窗口关闭
    }
    DestroyWindow(hwnd);
}
void BrowserWindow::setMinMaxInfo(MINMAXINFO* mmi)
{
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (hMonitor) {
        MONITORINFO mi = { sizeof(mi) };
        if (GetMonitorInfo(hMonitor, &mi)) {
            RECT workArea = mi.rcWork;
            RECT monitorArea = mi.rcMonitor;
            mmi->ptMaxPosition.x = workArea.left - monitorArea.left;
            mmi->ptMaxPosition.y = workArea.top - monitorArea.top;
            mmi->ptMaxSize.x = workArea.right - workArea.left;
            mmi->ptMaxSize.y = workArea.bottom - workArea.top;
        }
    }
    mmi->ptMinTrackSize.x = config->minWidth;
    mmi->ptMinTrackSize.y = config->minHeight;
    if (!config->maximizable) {
        mmi->ptMaxTrackSize.x = config->maxWidth;
        mmi->ptMaxTrackSize.y = config->maxHeight;
    }
}
void BrowserWindow::routeMsgToPage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (!ctrlComp) return;
    DWORD mouseData = 0;
    POINT point{ .x{GET_X_LPARAM(lParam)},.y{GET_Y_LPARAM(lParam)} };
    //todo 鼠标按下 释放时 SetCapture  ReleaseCapture
    if (msg == WM_MOUSEMOVE)
    {
        if (!isMouseTracking) {
            TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            isMouseTracking = true;
        }
    }
    else if (msg == WM_MOUSEWHEEL || msg == WM_MOUSEHWHEEL)
    {
        mouseData = GET_WHEEL_DELTA_WPARAM(wParam);
        ScreenToClient(hwnd, &point);
    }
    else if (msg == WM_XBUTTONDBLCLK || msg == WM_XBUTTONDOWN || msg == WM_XBUTTONUP) { //前进后退之类的按钮
        mouseData = GET_XBUTTON_WPARAM(wParam);
    }
    auto eventKind = static_cast<COREWEBVIEW2_MOUSE_EVENT_KIND>(msg);
    auto eventKey = static_cast<COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS>(GET_KEYSTATE_WPARAM(wParam));
    ctrlComp->SendMouseInput(eventKind, eventKey, mouseData, point);
}
std::wstring& BrowserWindow::getWinClsName()
{
    static std::wstring clsName = [] {
        WNDCLASSEXW wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = &BrowserWindow::winProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = App::get()->hInstance;
        wcex.hIcon = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_WINLOGO);
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = L"HorseJs";
        wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_WINLOGO);
        RegisterClassExW(&wcex);
        return wcex.lpszClassName;
        }();
    return clsName;
}

int BrowserWindow::hittest(const POINT& pt)
{
    if(!framelessResizable) return HTCLIENT;
    RECT rcClient;
    GetClientRect(hwnd, &rcClient);
    const int border = 6;
    bool left = pt.x < border;
    bool right = pt.x >= rcClient.right - border;
    bool top = pt.y < border;
    bool bottom = pt.y >= rcClient.bottom - border;
    if (top && left) return HTTOPLEFT;
    if (top && right) return HTTOPRIGHT;
    if (bottom && left) return HTBOTTOMLEFT;
    if (bottom && right) return HTBOTTOMRIGHT;
    if (left) return HTLEFT;
    if (right) return HTRIGHT;
    if (top) return HTTOP;
    if (bottom) return HTBOTTOM;
    return HTCLIENT;
}

LRESULT BrowserWindow::winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto winObj = reinterpret_cast<BrowserWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (winObj != nullptr)
    {
        return winObj->winMsg(hwnd, msg, wParam, lParam);
    }
    else {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

bool BrowserWindow::load(const rapidjson::Value& pageConfig)
{
    page = std::make_unique<Page>(this);
    page->init(pageConfig);
    auto ctrlReadyCB = WRL::Callback<ICoreWebView2CreateCoreWebView2CompositionControllerCompletedHandler>(this, &BrowserWindow::ctrlReady);
    auto env3 = App::get()->env.try_query<ICoreWebView2Environment3>();
    auto result = env3->CreateCoreWebView2CompositionController(hwnd,ctrlReadyCB.Get());
    if (FAILED(result)) {
        return false;
    }
    return true;
}

HRESULT BrowserWindow::ctrlReady(HRESULT result, ICoreWebView2CompositionController* ctrlComp)
{
    this->ctrlComp = ctrlComp;
    this->ctrl = this->ctrlComp.query<ICoreWebView2Controller>();
    ctrl->put_IsVisible(true);
    EventRegistrationToken token;
    auto cursorChangeCB = WRL::Callback<ICoreWebView2CursorChangedEventHandler>(this, &BrowserWindow::cursorChange);
    this->ctrlComp->add_CursorChanged(cursorChangeCB.Get(), &token);
    bindCompCtrlToHwnd();
    page->load();
    return S_OK;
}

HRESULT BrowserWindow::cursorChange(ICoreWebView2CompositionController*, IUnknown*)
{
    HCURSOR cursor = nullptr;
    HRESULT hr = this->ctrlComp->get_Cursor(&cursor);
    if (SUCCEEDED(hr) && cursor)
    {
        SetCursor(cursor);
    }
    return S_OK;
}

void BrowserWindow::setWindowStyle(long& exStyle, long& style)
{
    if (config->frame)
    {
        style = WS_OVERLAPPEDWINDOW;
    }
    else {
        style = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    }
    if (config->visible) {
        style = style | WS_VISIBLE;
    }
    if (!config->resizable) {
        style = style & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
    }
    if (!config->maximizable) {
        style = style & ~WS_MAXIMIZEBOX;
    }
    if (!config->minimizable) {
        style = style & ~WS_MINIMIZEBOX;
    }
    if (config->skipTaskbar) {
        exStyle = WS_EX_TOOLWINDOW;
    }
    else {
        exStyle = WS_EX_APPWINDOW;
    }
    if (config->alwaysOnTop) {
        exStyle = exStyle | WS_EX_TOPMOST;
    }
}

void BrowserWindow::bindCompCtrlToHwnd()
{
    namespace abi = ABI::Windows::System;
    DispatcherQueueOptions options{
            sizeof(DispatcherQueueOptions), /* dwSize */
            DQTYPE_THREAD_CURRENT,          /* threadType */
            DQTAT_COM_ASTA                  /* apartmentType */
    };
    winrt::Windows::System::DispatcherQueueController controller{ nullptr };
    CreateDispatcherQueueController(options, reinterpret_cast<abi::IDispatcherQueueController**>(winrt::put_abi(controller)));
    m_dispatcherQueueController = controller;
    m_compositor = winrt::Windows::UI::Composition::Compositor();

    namespace abi2 = ABI::Windows::UI::Composition::Desktop;

    auto interop = m_compositor.as<abi2::ICompositorDesktopInterop>();
    interop->CreateDesktopWindowTarget(hwnd, false, reinterpret_cast<abi2::IDesktopWindowTarget**>(winrt::put_abi(m_target)));

    m_rootVisual = m_compositor.CreateContainerVisual();
    m_rootVisual.RelativeSizeAdjustment({ 1.0f, 1.0f });
    m_rootVisual.Offset({ 0, 0, 0 });
    m_target.Root(m_rootVisual);

    m_webViewVisual = m_compositor.CreateContainerVisual();
    m_rootVisual.Children().InsertAtTop(m_webViewVisual);
    this->ctrlComp->put_RootVisualTarget(m_webViewVisual.as<IUnknown>().get());
    RECT bounds;
    GetClientRect(hwnd, &bounds);
    ctrl->put_Bounds(bounds);
}