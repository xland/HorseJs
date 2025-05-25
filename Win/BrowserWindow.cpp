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
}

void BrowserWindow::unregEvent(const int& eventId)
{
    if (eventId == (int)WindowEventId::closing) {
        closingIsReg = false;
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
        POINT point;
        point.x = GET_X_LPARAM(lParam);
        point.y = GET_Y_LPARAM(lParam);
        ctrlComp->SendMouseInput(static_cast<COREWEBVIEW2_MOUSE_EVENT_KIND>(msg),
            static_cast<COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS>(GET_KEYSTATE_WPARAM(wParam)), 0,point);
        return true;
    }
    switch (msg)
    {
        case WM_SETCURSOR:
        {
            if (ctrlComp)
            {
                HCURSOR cursor = nullptr;
                if (SUCCEEDED(ctrlComp->get_Cursor(&cursor)) && cursor)
                {
                    ::SetCursor(cursor);
                    return TRUE;
                }
            }
            break;
        }
        case WM_CLOSE: {
            if (closingIsReg) {
                msgProcessor->emit((int)ClassId::Window, (int)WindowEventId::closing, 0);
                return 0;
            }
            DestroyWindow(hwnd);
            return 0;
        }
        case WM_SIZE: {
            RECT bounds;
            GetClientRect(hwnd, &bounds);
            config->w = bounds.right - bounds.left;
            config->h = bounds.bottom - bounds.top;
            if (ctrl) {
                ctrl->SetBoundsAndZoomFactor(bounds, 1.0);
            }
/*            if (sizedIsReg) {
                msgProcessor->emit((int)ClassId::Window, (int)WindowEventId::sized, 2,config->w,config->h);
            }      */      
            return 0;
        }
        case WM_DESTROY: {
            KillTimer(hwnd, 100);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);            
            App::get()->onWindowDestroy(this);
            return 0;
        }
        case WM_GETMINMAXINFO:
        {
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
    }
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
    page->load();
    RECT bounds;
    GetClientRect(hwnd, &bounds);

    EventRegistrationToken token;
    auto cursorChangeCB = WRL::Callback<ICoreWebView2CursorChangedEventHandler>(this, &BrowserWindow::cursorChange);
    this->ctrlComp->add_CursorChanged(cursorChangeCB.Get(), &token);

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

    ctrl->put_Bounds(bounds);

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

void BrowserWindow::call(rapidjson::Document& jsonDoc)
{
    int methodId{ -1 }, eventId{ -1 };
    if (jsonDoc.HasMember("methodId") && jsonDoc["methodId"].IsInt()) {
        methodId = jsonDoc["methodId"].GetInt();
    }
    if (jsonDoc.HasMember("eventId") && jsonDoc["eventId"].IsInt()) {
        eventId = jsonDoc["eventId"].GetInt();
    }
    if (methodId < 0 || eventId < 0) {
        MessageBox(nullptr, L"methodId或eventId为空", L"错误", MB_OK | MB_ICONERROR);
        return;
    }
}

void BrowserWindow::resize(const int& w, const int& h)
{
    SetWindowPos(hwnd,nullptr,0,0,w,h,SWP_NOMOVE | SWP_NOZORDER);
}