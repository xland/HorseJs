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
        config->x, config->y, config->w, config->h, nullptr, nullptr, wcex->hInstance, nullptr);
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
        case WM_TIMER: {
            if (wParam == 100) {
                POINT pt;
                GetCursorPos(&pt);
                RECT rc;
                GetClientRect(hwnd, &rc);  // rc 是客户区的大小（相对于客户区左上角）

                // 将客户区左上角转换为屏幕坐标
                POINT topLeft = { rc.left, rc.top };
                ClientToScreen(hwnd, &topLeft);

                // 将客户区右下角转换为屏幕坐标（可选）
                POINT bottomRight = { rc.right, rc.bottom };
                ClientToScreen(hwnd, &bottomRight);

                if (pt.x>topLeft.x && pt.y>topLeft.y && pt.x < bottomRight.x && pt.y < bottomRight.y)
                {
                    if(ctrlComp.get()){
                        ctrlComp->SendMouseInput(
                            COREWEBVIEW2_MOUSE_EVENT_KIND_LEAVE,
                            COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS_NONE,
                            0,
                            pt );
                    }
                }
            }
            break;
        }
        case WM_WINDOWPOSCHANGED:
        {
            WINDOWPLACEMENT wp = {};
            wp.length = sizeof(wp);
            GetWindowPlacement(hwnd, &wp);
            if (wp.showCmd == SW_SHOWNORMAL || wp.showCmd == SW_RESTORE) {
                if (IsWindowVisible(hwnd)) {
                    SetTimer(hwnd, 100, 1000, NULL);
                }
                else {
                    KillTimer(hwnd, 100);
                }
            }
            else if (wp.showCmd == SW_SHOWMAXIMIZED|| wp.showCmd == SW_SHOWMINIMIZED) {
                KillTimer(hwnd, 100);
            }
            break;
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
  //      case WM_WINDOWPOSCHANGED:{
  //          WINDOWPOS* wp = (WINDOWPOS*)lParam;
  //          if (wp->flags & SWP_NOSIZE) {
  //              return 0;
  //          }
  //          RECT bounds;
  //          GetClientRect(hwnd, &bounds);
  //          config->w = bounds.right - bounds.left;
  //          config->h = bounds.bottom - bounds.top;
  //          if (ctrl) {
  //              ctrl->SetBoundsAndZoomFactor(bounds, 1.0);
  //          }
  //          return 0;
		//}
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

    auto app = App::get();
    auto ctrlReadyCB = WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(this, &BrowserWindow::ctrlReady);
    auto result = app->env->CreateCoreWebView2Controller(hwnd, ctrlReadyCB.Get());
    if (FAILED(result)) {
        return false;
    }
    return true;
}

HRESULT BrowserWindow::ctrlReady(HRESULT result, ICoreWebView2Controller* ctrl)
{
    this->ctrl = ctrl;
    //ctrlComp = this->ctrl.try_query<ICoreWebView2CompositionController>();
    //auto m_compositionController = this->ctrl.try_query<ICoreWebView2CompositionController>();
    //m_compositionController->SendMouseInput(
    //    static_cast<COREWEBVIEW2_MOUSE_EVENT_KIND>(WM_MOUSELEAVE),
    //    static_cast<COREWEBVIEW2_MOUSE_EVENT_VIRTUAL_KEYS>(GET_KEYSTATE_WPARAM(wParam)),
    //    mouseData, point));
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