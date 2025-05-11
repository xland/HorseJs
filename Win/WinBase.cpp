#include <windowsx.h>
#include <dwmapi.h>

#include "WinBase.h"
#include "Page.h"
#include "../App/App.h"

using namespace Microsoft::WRL;

WinBase::WinBase(const int &x, const int &y, const int &w, const int &h,
                 const bool &visible, const bool& frame, const bool& shadow,
                 const std::wstring &title)
    : x{x}, y{y}, w{w}, h{h},
	visible{ visible }, frame{ frame }, shadow{ shadow },
    title{ title }
{
    initWindow();
    createPageCtrl();
}

WinBase::~WinBase()
{
}

WinBase *WinBase::create(const rapidjson::Value &config)
{
    int x{100}, y{100}, w{1000}, h{800};
    bool maximize{ false }, visible{ true }, frame{ true }, shadow{true};
    std::wstring title{L"Window - HorseJs"};
    if (config.HasMember("size"))
    {
        if (config["size"].IsString())
        {
            auto size = std::string_view(config["size"].GetString());
            if (size == "maximize")
            {
                maximize = true;
            }
        }
        else if (config["size"].IsObject())
        {
            auto sizeObj = config["size"].GetObj();
            w = sizeObj["w"].GetInt();
            h = sizeObj["h"].GetInt();
        }
    }
    if (config.HasMember("position") && config["position"].IsString())
    {
        auto pos = std::string_view(config["position"].GetString());
        if (pos == "screenCenter")
        {
            int sw = GetSystemMetrics(SM_CXSCREEN);
            int sh = GetSystemMetrics(SM_CYSCREEN);
            x = (sw - w) / 2;
            y = (sh - h) / 2;
        }
    }
    if (config.HasMember("visible") && config["visible"].IsBool())
    {
        visible = config["visible"].GetBool();
    }
    if (config.HasMember("frame") && config["frame"].IsBool())
    {
        frame = config["frame"].GetBool();
    }
    if (config.HasMember("shadow") && config["shadow"].IsBool())
    {
        shadow = config["shadow"].GetBool();
    }
    if (config.HasMember("title") && config["title"].IsString())
    {
        title = Util::convertToWStr(config["title"].GetString());
    }
    return new WinBase(x, y, w, h, visible, frame,shadow,title);
}


void WinBase::initWindow()
{
    auto wcex = regWinClass();
    long winStyle;
    if (frame)
    {
        winStyle = WS_OVERLAPPEDWINDOW;
    }
    else {
		winStyle = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    }
    if (visible) {
        winStyle = winStyle | WS_VISIBLE;
    }
    //WS_EX_APPWINDOW 确保窗口出现在任务栏
    hwnd = CreateWindowEx(WS_EX_APPWINDOW, wcex->lpszClassName, title.data(), winStyle, x, y, w, h, nullptr, nullptr, wcex->hInstance, this);
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
WNDCLASSEX *WinBase::regWinClass()
{
    static WNDCLASSEX wcex;
    static bool wcexInit = false;
    if (!wcexInit)
    {
        auto hinstance = GetModuleHandle(NULL);
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = &WinBase::winProc;
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

void WinBase::show()
{
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
}

LRESULT WinBase::winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto winObj = reinterpret_cast<WinBase *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (winObj != nullptr)
    {
        if (msg == WM_CLOSE) {
            DestroyWindow(hwnd);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
            delete winObj;
        }
        else {
			return winObj->winMsg(hwnd, msg, wParam, lParam);
        }
    }
    else {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

LRESULT WinBase::winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_SIZE: {
            if (ctrl) {
                // 获取新的窗口客户区大小
                RECT bounds;
                GetClientRect(hwnd, &bounds);
                ctrl->SetBoundsAndZoomFactor(bounds, 1.0);
            }
            return 0;
        }
        case WM_NCHITTEST: {
            LRESULT hit = DefWindowProc(hwnd, msg, wParam, lParam);
            if (hit == HTCLIENT) {
                POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                ScreenToClient(hwnd, &pt);

                RECT rect;
                GetClientRect(hwnd, &rect);

                bool left = pt.x < 5;
                bool right = pt.x > rect.right - 5;
                bool top = pt.y < 5;
                bool bottom = pt.y > rect.bottom - 5;

                if (left && top) return HTTOPLEFT;
                if (right && top) return HTTOPRIGHT;
                if (left && bottom) return HTBOTTOMLEFT;
                if (right && bottom) return HTBOTTOMRIGHT;
                if (left) return HTLEFT;
                if (right) return HTRIGHT;
                if (top) return HTTOP;
                if (bottom) return HTBOTTOM;

                return HTCAPTION;
            }
            return hit;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool WinBase::createPageCtrl()
{
    auto app = App::get();
    auto callBackInstance = Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(this, &WinBase::pageCtrlReady);
    auto result = app->env->CreateCoreWebView2Controller(hwnd, callBackInstance.Get());
    if (FAILED(result)) {
        return false;
    }
    return true;
}

HRESULT WinBase::pageCtrlReady(HRESULT result, ICoreWebView2Controller* ctrl)
{
    HRESULT hr;
    this->ctrl = ctrl;
    hr = ctrl->get_CoreWebView2(&webview);

    wil::com_ptr<ICoreWebView2Settings> settings;
    webview->get_Settings(&settings);
    settings->put_IsScriptEnabled(TRUE);
    settings->put_AreDefaultScriptDialogsEnabled(TRUE);
    settings->put_IsWebMessageEnabled(TRUE);

    
    auto navigateCB = Callback<ICoreWebView2NavigationStartingEventHandler>(this, &WinBase::navigationStarting);
    EventRegistrationToken navigateToken;
    webview->add_NavigationStarting(navigateCB.Get(), &navigateToken);

    auto titleChangedCB = Callback<ICoreWebView2DocumentTitleChangedEventHandler>(this, &WinBase::titleChanged);
    EventRegistrationToken titleToken;
    hr = webview->add_DocumentTitleChanged(titleChangedCB.Get(), &titleToken);

    auto statusChangeCB = Callback<ICoreWebView2StatusBarTextChangedEventHandler>(this, &WinBase::statusChanged);
    EventRegistrationToken statusToken;
    auto webView12 = webview.try_query<ICoreWebView2_12>();
    hr = webView12->add_StatusBarTextChanged(statusChangeCB.Get(), &statusToken);

    
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken_;
    Gdiplus::GdiplusStartup(&gdiplusToken_, &gdiplusStartupInput, NULL);
    auto webView15 = webview.try_query<ICoreWebView2_15>();
    EventRegistrationToken faviconToken;
    auto faviconChangeCB = Callback<ICoreWebView2FaviconChangedEventHandler>(this, &WinBase::faviconChange);
    hr = webView15->add_FaviconChanged(faviconChangeCB.Get(),&faviconToken);

    webview->Navigate(L"https://www.microsoft.com/");

    //RECT bounds{ .left{0}, .top{0}, .right{w}, .bottom{h} };
    RECT bounds;
    GetClientRect(hwnd, &bounds);
    hr = ctrl->put_Bounds(bounds);
    return hr;
}

HRESULT WinBase::navigationStarting(ICoreWebView2* webview, ICoreWebView2NavigationStartingEventArgs* args)
{
    return S_OK;
}

HRESULT WinBase::titleChanged(ICoreWebView2* sender, IUnknown* args)
{
	LPWSTR titleData;
    HRESULT hr = webview->get_DocumentTitle(&titleData);
	title = titleData;
    SetWindowText(hwnd, title.data());
    CoTaskMemFree(titleData);
    return S_OK;
}

HRESULT WinBase::statusChanged(ICoreWebView2* sender, IUnknown* args)
{
    LPWSTR statusData;
    auto m_webView2_12 = webview.try_query<ICoreWebView2_12>();
    HRESULT hr = m_webView2_12->get_StatusBarText(&statusData);
    CoTaskMemFree(statusData);
    return S_OK;
}

HRESULT WinBase::faviconChange(ICoreWebView2* sender, IUnknown* args)
{
    auto webView15 = webview.try_query<ICoreWebView2_15>();
    LPWSTR urlData;
    webView15->get_FaviconUri(&urlData);
    std::wstring url = urlData;
    CoTaskMemFree(urlData);
    webView15->GetFavicon(COREWEBVIEW2_FAVICON_IMAGE_FORMAT_PNG,
        Callback<ICoreWebView2GetFaviconCompletedHandler>([this, url](HRESULT errorCode, IStream* iconStream)
            {
                Gdiplus::Bitmap iconBitmap(iconStream);
                wil::unique_hicon icon;
                auto hr = iconBitmap.GetHICON(&icon);
                if (hr == Gdiplus::Status::Ok)
                {
                    favicon = std::move(icon);
                    SendMessage(hwnd, WM_SETICON,ICON_SMALL, (LPARAM)favicon.get());
                }
                else
                {
                    SendMessage(hwnd, WM_SETICON,ICON_SMALL, (LPARAM)IDC_NO);
                }
                return S_OK;
            }).Get());
    return S_OK;
}
