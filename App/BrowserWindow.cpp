
#include <pch.h>

#include "../App/App.h"
#include "../Processor/MsgProcessor.h"
#include "../Processor/Tray.h"
#include "BrowserWindow.h"

BrowserWindow::BrowserWindow(const rapidjson::Value& winConfig)
{
    configSize(winConfig);
    configPos(winConfig);
    configPage(winConfig);
    configOther(winConfig);
    initWindow();
}

BrowserWindow::~BrowserWindow()
{
    for (auto& tray:trays)
    {
        Shell_NotifyIcon(NIM_DELETE, tray);
        delete tray;
    }
    for (auto& item :trayMenus)
    {
        DestroyMenu(item.second);
    }
}

void BrowserWindow::initWindow()
{
    long exStyle, style;
    setWindowStyle(exStyle, style);
    hwnd = CreateWindowEx(exStyle, getWinClsName().data(), title.data(), style,
        x, y, w, h, nullptr, nullptr, App::get()->hInstance, nullptr);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    if (!frame && shadow)
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
    else if (msg == WM_THREAD_RESULT) {
        auto result = reinterpret_cast<JsonResult*>(lParam);
        result->returnBack();
        delete result;
    }
    else if (msg == WM_COMMAND) {
        JsonResult result(id, "tray", std::to_string(wParam));
        result.returnBack();
    }
    else if (msg == WM_TRAY) {
        if (lParam == WM_RBUTTONDOWN) {
            JsonResult result(id, "tray", std::to_string(wParam));
            result.addString("type", "rightBtnDown");
            result.returnBack();
            POINT pt;
            GetCursorPos(&pt);
            if (trayMenus.contains(wParam)) {
                SetForegroundWindow(hwnd);
                TrackPopupMenuEx(trayMenus[wParam], TPM_RIGHTBUTTON, pt.x, pt.y, hwnd, nullptr);
            }
        }
        else if (lParam == WM_LBUTTONDOWN) {
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            JsonResult result(id, "tray", std::to_string(wParam));
            result.addString("type", "leftBtnDown");
            result.returnBack();
        }
    }
    sysProcess:
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void BrowserWindow::sizePosChanged(WINDOWPOS* winPos)
{
    x = winPos->x;
    y = winPos->y;
    w = winPos->cx;
    h = winPos->cy;
    if (!ctrl) return;
    RECT bounds;
    GetClientRect(hwnd, &bounds);
    ctrl->put_Bounds(bounds);
    auto& vec = events["sizePosChanged"];
    if (vec.size() == 0) return;
    for (auto& id:vec)
    {
        JsonResult result(id, "win", "sizePosChanged");
        result.addNumber("x", x);
        result.addNumber("y", y);
        result.addNumber("w", w);
        result.addNumber("h", h);
        result.returnBack();
    }
}
void BrowserWindow::stateChanged(const int& state)
{
    if (!ctrl) return;
    auto& vec = events["stateChanged"];
    if (vec.size() == 0) return;
    for (auto& id : vec)
    {
        JsonResult result(id, "win", "stateChanged");
        if (state == SIZE_MAXIMIZED) {
            result.addString("state", "maximize");
        }
        else if (state == SIZE_MINIMIZED) {
            result.addString("state", "minimize");
        }
        else if (state == SIZE_RESTORED) {
            result.addString("state", "restore");
        }
        result.returnBack();
    }
}
void BrowserWindow::closing()
{
    auto& vec = events["closing"];
    if (vec.size() > 0) {
        for (auto& id : vec)
        {
            JsonResult result(id, "win", "sizePosChanged");
            result.returnBack();
        }
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
    mmi->ptMinTrackSize.x = minWidth;
    mmi->ptMinTrackSize.y = minHeight;
    if (!maximizable) {
        mmi->ptMaxTrackSize.x = maxWidth;
        mmi->ptMaxTrackSize.y = maxHeight;
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

bool BrowserWindow::load()
{
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
    loadPage();
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
    if (frame)
    {
        style = WS_OVERLAPPEDWINDOW;
    }
    else {
        style = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    }
    if (visible) {
        style = style | WS_VISIBLE;
    }
    if (!resizable) {
        style = style & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
    }
    if (!maximizable) {
        style = style & ~WS_MAXIMIZEBOX;
    }
    if (!minimizable) {
        style = style & ~WS_MINIMIZEBOX;
    }
    if (skipTaskbar) {
        exStyle = WS_EX_TOOLWINDOW;
    }
    else {
        exStyle = WS_EX_APPWINDOW;
    }
    if (alwaysOnTop) {
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