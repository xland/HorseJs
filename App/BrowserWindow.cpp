
#include <pch.h>

#include "../App/App.h"
#include "../Processor/MsgProcessor.h"
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

    // 准备内存 DC 和位图
    //HDC hdcScreen = GetDC(NULL);
    //HDC hdcMem = CreateCompatibleDC(hdcScreen);
    //HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, w, h);
    //SelectObject(hdcMem, hBitmap);
    //BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    //POINT ptSrc = { 0, 0 };
    //SIZE sizeWnd = { w, h };
    //UpdateLayeredWindow(hwnd, hdcScreen, NULL, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
    //DeleteDC(hdcMem);
    //ReleaseDC(NULL, hdcScreen);
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

    //todo 
    //auto ctrl2 = ctrl.try_query<ICoreWebView2Controller2>();
    //COREWEBVIEW2_COLOR bgColor = { 60, 30, 30, 30 };
    //ctrl2->put_DefaultBackgroundColor(bgColor);

    EventRegistrationToken token;
    auto cursorChangeCB = WRL::Callback<ICoreWebView2CursorChangedEventHandler>(this, &BrowserWindow::cursorChange);
    this->ctrlComp->add_CursorChanged(cursorChangeCB.Get(), &token);
    bindCompCtrlToHwnd();
    loadPage();
    return S_OK;
}

void BrowserWindow::setWindowStyle(long& exStyle, long& style)
{
    if (frame)
    {
        style = WS_OVERLAPPEDWINDOW;
    }
    else {
        style = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS ;
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
        //exStyle = WS_EX_LAYERED;
    }
    if (alwaysOnTop) {
        exStyle = exStyle | WS_EX_TOPMOST;
    }
}

void BrowserWindow::bindCompCtrlToHwnd()
{
    auto app = App::get();
    auto interop = app->compositor.as<ABI::Windows::UI::Composition::Desktop::ICompositorDesktopInterop>();
    interop->CreateDesktopWindowTarget(hwnd, false, reinterpret_cast<ABI::Windows::UI::Composition::Desktop::IDesktopWindowTarget**>(winrt::put_abi(m_target)));

    m_rootVisual = app->compositor.CreateContainerVisual();
    m_rootVisual.RelativeSizeAdjustment({ 1.0f, 1.0f });
    m_rootVisual.Offset({ 0, 0, 0 });
    m_target.Root(m_rootVisual);

    m_webViewVisual = app->compositor.CreateContainerVisual();
    m_rootVisual.Children().InsertAtTop(m_webViewVisual);
    this->ctrlComp->put_RootVisualTarget(m_webViewVisual.as<IUnknown>().get());
    RECT bounds;
    GetClientRect(hwnd, &bounds);
    ctrl->put_Bounds(bounds);
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