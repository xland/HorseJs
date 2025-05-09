#include "WinBase.h"

WinBase::WinBase(const int &x, const int &y, const int &w, const int &h,
                 const bool &visible, const std::wstring &title)
    : x{x}, y{y}, w{w}, h{h},
      visible{visible}, title{title}
{
    initWindow();
}

WinBase::~WinBase()
{
}

void WinBase::initWindow()
{
    auto wcex = regWinClass();
    auto winStyle = WS_OVERLAPPEDWINDOW;
    if (visible)
        winStyle = winStyle | WS_VISIBLE;

    hwnd = CreateWindowEx(WS_EX_APPWINDOW, wcex->lpszClassName, title.data(), winStyle, x, y, w, h, nullptr, nullptr, wcex->hInstance, this);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

WinBase *WinBase::create(const rapidjson::Value &config)
{
    int x{100}, y{100}, w{1000}, h{800};
    bool maximize{false}, visible{true};
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
    if (config.HasMember("title") && config["title"].IsString())
    {
        title = Util::convertToWStr(config["title"].GetString());
    }
    return new WinBase(x, y, w, h, visible, title);
}

WNDCLASSEX *WinBase::regWinClass()
{
    static WNDCLASSEX wcex;
    static bool wcexInit = false;
    if (!wcexInit)
    {
        auto hinstance = GetModuleHandle(NULL);
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcex.lpfnWndProc = &WinBase::WinProc;
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

LRESULT WinBase::WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto winObj = reinterpret_cast<WinBase *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (winObj != nullptr)
    {
        switch (msg)
        {
        case WM_CLOSE:
        {
            DestroyWindow(hwnd);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
            delete winObj;
            return 0;
        }
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
