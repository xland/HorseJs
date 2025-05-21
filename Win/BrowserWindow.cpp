#include <windowsx.h>
#include <dwmapi.h>

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
    else if (eventId == (int)WindowEventId::sizing) {
        sizingIsReg = true;
    }
    else if (eventId == (int)WindowEventId::sized) {
        sizedIsReg = true;
    }
}

void BrowserWindow::unregEvent()
{
}

void BrowserWindow::initWindow()
{
    auto wcex = regWinClass();
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
    hwnd = CreateWindowEx(WS_EX_APPWINDOW, wcex->lpszClassName, config->title.data(), winStyle, 
        config->x, config->y, config->w, config->h, 
        nullptr, nullptr, wcex->hInstance, this);
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
WNDCLASSEX*  BrowserWindow::regWinClass()
{
    static WNDCLASSEX wcex;
    static bool wcexInit = false;
    if (!wcexInit)
    {
        auto hinstance = GetModuleHandle(NULL);
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = &BrowserWindow::winProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hinstance;
        wcex.hIcon = LoadIcon(hinstance, (LPCTSTR)IDI_WINLOGO);
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = L"HorseJs";
        wcex.hIconSm = LoadIcon(hinstance, (LPCTSTR)IDI_WINLOGO);
        if (!RegisterClassEx(&wcex))
        {
            MessageBox(NULL, L"注册窗口类失败", L"系统提示", NULL);
        }
        wcexInit = true;
    }
    return &wcex;
}

void BrowserWindow::show()
{
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
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
    switch (msg)
    {
        case WM_CLOSE: {
            if (closingIsReg) {
                msgProcessor->emit((int)ClassId::Window, (int)WindowEventId::closing, 0);
                return 0;
            }
            DestroyWindow(hwnd);
            return 0;
        }
        case WM_SIZE: {
            if (ctrl) {
                RECT bounds;
                GetClientRect(hwnd, &bounds);
                ctrl->SetBoundsAndZoomFactor(bounds, 1.0);
            }
            msgProcessor->emit((int)ClassId::Window, (int)WindowEventId::sized, 0);
            return 0;
        }
        case WM_SIZING: {
            return 0;
        }
        case WM_DESTROY: {
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

    auto app = App::get();
    auto ctrlReadyCB = WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(this, &BrowserWindow::pageCtrlReady);
    auto result = app->env->CreateCoreWebView2Controller(hwnd, ctrlReadyCB.Get());
    if (FAILED(result)) {
        return false;
    }
    return true;
}

HRESULT BrowserWindow::pageCtrlReady(HRESULT result, ICoreWebView2Controller* ctrl)
{
    this->ctrl = ctrl;
    page->load();
    RECT bounds;
    GetClientRect(hwnd, &bounds);
    auto hr = ctrl->put_Bounds(bounds);
    return hr;
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
