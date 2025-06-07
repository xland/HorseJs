#include "pch.h"
#include "App.h"
#include "MenuWindow.h"

namespace {
	std::unique_ptr<MenuWindow> hw;
}

MenuWindow::MenuWindow()
{
    createWindow();
}

MenuWindow::~MenuWindow()
{
}
MenuWindow* MenuWindow::get() {
	if (!hw) {
		hw = std::make_unique<MenuWindow>();
	}
	return hw.get();
}
void MenuWindow::show(const POINT& pt, std::map<int, std::wstring>& menus)
{
    ShowWindow(hwnd, SW_HIDE);
    w = 0;
    ids.clear();
    texts.clear();
    HDC hdc = GetDC(hwnd);
    for (const auto& pair : menus)
    {
        SIZE size;
        GetTextExtentPoint32(hdc, pair.second.c_str(), pair.second.length(), &size);
        if (size.cx > w) w = size.cx;
        ids.push_back(pair.first);
        texts.push_back(pair.second);
    }
    h = menus.size() * lineHeight;
    w += 80;
    ReleaseDC(hwnd, hdc);
    SetWindowPos(hwnd, NULL, pt.x-w/2, pt.y-h-6, w, h, SWP_SHOWWINDOW);
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
}
void MenuWindow::createWindow()
{
    WNDCLASS wc = { 0 };
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = &MenuWindow::winProc;
    wc.hInstance = App::get()->hInstance;
    wc.lpszClassName = L"MenuWindowClass";
    RegisterClass(&wc);
    hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, wc.lpszClassName, NULL, 
        WS_POPUP, 0, 0, 0, 0,NULL, NULL, wc.hInstance, NULL);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    MARGINS margins = { 1, 1, 1, 1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    int value = 2;
    DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &value, sizeof(value));
    DwmSetWindowAttribute(hwnd, DWMWA_ALLOW_NCPAINT, &value, sizeof(value));
}
LRESULT MenuWindow::winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto winObj = reinterpret_cast<MenuWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (winObj != nullptr)
    {
        return winObj->winMsg(hwnd, msg, wParam, lParam);
    }
    else {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
LRESULT MenuWindow::winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_PAINT)
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps); //todo 不支持选中的菜单项
        SetBkMode(hdc, TRANSPARENT);
        RECT rc = { 0, 0, w, h };
        FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW));
        for (int i = 0; i < texts.size(); ++i) {
            RECT rc = { 0, i * lineHeight, w, (i + 1) * lineHeight };
            if (i == hoverIndex) FillRect(hdc, &rc, (HBRUSH)(COLOR_MENUHILIGHT));
            DrawText(hdc, texts[i].c_str(), -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        }
        EndPaint(hwnd, &ps);
    }
    else if (msg == WM_MOUSEMOVE) {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(hwnd, &pt);
        int index = pt.y / lineHeight;
        if (index >= 0 && index < texts.size() && index != hoverIndex) {
            hoverIndex = index;
            InvalidateRect(hwnd, NULL, TRUE);
        }
    }
    else if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN) {
        POINT pt;
        GetCursorPos(&pt);

    }
    else if (msg == WM_ACTIVATE) {
        if (wParam == WA_INACTIVE) {
            ShowWindow(hwnd, SW_HIDE);
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}