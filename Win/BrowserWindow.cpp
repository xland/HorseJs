#include <windowsx.h>
#include <dwmapi.h>
#include <winrt/base.h>

#include "../App/App.h"
#include "BrowserWindow.h"
#include "BrowserWindowConfig.h"
#include "Page.h"
#include "MsgProcessor.h"
#include "EnumId.h"

using namespace Microsoft;

BrowserWindow::BrowserWindow(rapidjson::Value& winConfig)
    : config{std::make_unique<BrowserWindowConfig>(winConfig)}
{
    initWindow();
}

BrowserWindow::~BrowserWindow()
{
}


void BrowserWindow::regEvent(const int& eventId)
{
    if (eventId == (int)WindowEventId::closing) {
        closingIsReg = true;
    }
    else if (eventId == (int)WindowEventId::sizePosChanged) {
        sizePosChangedIsReg = true;
    }
}

void BrowserWindow::unregEvent(const int& eventId)
{
    if (eventId == (int)WindowEventId::closing) {
        closingIsReg = false;
    }
    else if (eventId == (int)WindowEventId::sizePosChanged) {
        sizePosChangedIsReg = false;
    }
}

void BrowserWindow::maximize()
{
    ShowWindow(hwnd, SW_MAXIMIZE);
}

void BrowserWindow::minimize()
{
    ShowWindow(hwnd, SW_MINIMIZE);
}
void BrowserWindow::show()
{
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    //ShowWindow(hwnd, SW_SHOWNORMAL);
}

void BrowserWindow::hide()
{
    ShowWindow(hwnd, SW_HIDE);
}

void BrowserWindow::restore()
{
    ShowWindow(hwnd, SW_RESTORE);
}

void BrowserWindow::close()
{
    //CloseWindow(hwnd); 这相当于窗口最小化
    //不能用SendMessage，因为这回导致对象删除之后，MsgProcessor还在准备向页面发消息
    PostMessage(hwnd, WM_CLOSE, 0, 0);
}

void BrowserWindow::destroy()
{
    PostMessage(hwnd, WM_DESTROY, 0, 0);
}

void BrowserWindow::flash(bool isStart)
{
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

void BrowserWindow::setResizable(bool flag)
{
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

void BrowserWindow::initWindow()
{
    long winStyle;
    if (config->frame)
    {
        winStyle = WS_OVERLAPPEDWINDOW;
    }
    else {
		winStyle = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    }
    if (config->visible) {
        winStyle = winStyle | WS_VISIBLE;
    }
    if (!config->resizable) {
        winStyle = winStyle & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
    }
    //WS_EX_APPWINDOW 确保窗口出现在任务栏
    hwnd = CreateWindowEx(WS_EX_APPWINDOW, getWinClsName().data(), config->title.data(), winStyle,
        config->x, config->y, config->w, config->h, nullptr, nullptr, App::get()->hInstance, nullptr);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    SetTimer(hwnd, 100, 1000, NULL);
    if (!config->frame && config->shadow)
    {
        MARGINS margins = { 1, 1, 1, 1 };
        DwmExtendFrameIntoClientArea(hwnd, &margins);
        int value = 2;
        DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &value, sizeof(value));
        DwmSetWindowAttribute(hwnd, DWMWA_ALLOW_NCPAINT, &value, sizeof(value));
    }

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

LRESULT BrowserWindow::winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST)
    {
        if (!ctrlComp) goto sysProcess;
        if (msg == WM_MOUSEMOVE && !isMouseTracking)
        {
            TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            TrackMouseEvent(&tme);
            isMouseTracking = true;
        }
        POINT point{ .x{GET_X_LPARAM(lParam)},.y{GET_Y_LPARAM(lParam)} };
        ctrlComp->SendMouseInput(static_cast<COREWEBVIEW2_MOUSE_EVENT_KIND>(msg),
            static_cast<COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS>(GET_KEYSTATE_WPARAM(wParam)), 0, point);
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
    else if (msg == WM_SIZE) {
        if (!ctrl) goto sysProcess;
        RECT bounds;
        GetClientRect(hwnd, &bounds);
        ctrl->put_Bounds(bounds);
        return false;
    }
    else if (msg == WM_CLOSE) {
        if (closingIsReg) {
            msgProcessor->emit((int)ClassId::Window, (int)WindowEventId::closing, 0);
            return false; //阻止窗口关闭
        }
        DestroyWindow(hwnd);
        return false;
    }
    else if (msg == WM_DESTROY) {
        SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
        msgProcessor->win = nullptr;
        msgProcessor->page = nullptr;
        App::get()->onWindowDestroy(this);
        return false;
    }
    else if (msg == WM_GETMINMAXINFO) {
        MINMAXINFO* mminfo = (PMINMAXINFO)lParam;
        RECT workArea;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
        mminfo->ptMinTrackSize.x = 600;
        mminfo->ptMinTrackSize.y = 400;
        if (!config->maximizable) {
            //mminfo->ptMaxSize.x = workArea.right - workArea.left - 2;
            //mminfo->ptMaxSize.y = workArea.bottom - workArea.top - 2;
            //mminfo->ptMaxPosition.x = 1;
            //mminfo->ptMaxPosition.y = 1;
        }
        return true;
    }
    else if (msg == WM_WINDOWPOSCHANGED) {
        WINDOWPOS* pos = reinterpret_cast<WINDOWPOS*>(lParam);
        config->x = pos->x;
        config->y = pos->y;
        config->w = pos->cx;
        config->h = pos->cy;
        if (sizePosChangedIsReg) {
            msgProcessor->emit((int)ClassId::Window, (int)WindowEventId::sizePosChanged, 4, config->x, config->y, config->w, config->h);
        }        
    }
    sysProcess:
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool BrowserWindow::load(rapidjson::Value& pageConfig)
{
    page = std::make_unique<Page>(this);
    page->init(pageConfig);
    msgProcessor = std::make_unique<MsgProcessor>(this, page.get());
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

void BrowserWindow::resize(const int& w, const int& h)
{
    SetWindowPos(hwnd,nullptr,0,0,w,h,SWP_NOMOVE | SWP_NOZORDER);
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