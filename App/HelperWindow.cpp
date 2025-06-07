#include "pch.h"
#include "HelperWindow.h"
#include "../Processor/Tray.h"
#include "App.h"
namespace {
	std::unique_ptr<HelperWindow> hw;
    static std::atomic<bool> hwndReady{ false };
}

HelperWindow::HelperWindow()
{
    std::jthread worker(&HelperWindow::createWindow, this);
    worker.detach();
}

void HelperWindow::createWindow()
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = &HelperWindow::winProc;
    wc.hInstance = App::get()->hInstance;
    wc.lpszClassName = L"HelperWindowClass";
    RegisterClass(&wc);
    hwnd = CreateWindow(wc.lpszClassName, L"HelperWindow", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr, wc.hInstance, nullptr);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    hwndReady = true;
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void HelperWindow::createTray(TrayData* trayData)
{
    NOTIFYICONDATA* tray = new NOTIFYICONDATA();
    ZeroMemory(tray, sizeof(NOTIFYICONDATA));
    tray->cbSize = sizeof(NOTIFYICONDATA);
    tray->hWnd = hwnd;
    tray->uID = trayData->id;
    tray->uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    tray->uCallbackMessage = WM_TRAY;
    tray->hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcscpy_s(tray->szTip, trayData->tip.data());
    Shell_NotifyIcon(NIM_ADD, tray);
    trays.push_back(tray);
    auto& ms = trayData->menus;
    if(!ms.empty()){
        HMENU menu = CreatePopupMenu();
        for (const auto& pair : ms)
        {
            auto str = pair.second;
            AppendMenu(menu, MF_STRING, pair.first, L"测试数据测试");
        }
        menus.insert({ tray->uID,menu });
    }
    trayWinId.insert({ tray->uID,trayData->winId });
    //delete trayData;
}

HelperWindow::~HelperWindow()
{
}
HelperWindow* HelperWindow::get() {
	if (!hw) {
		hw = std::make_unique<HelperWindow>();
	}
	return hw.get();
}
LRESULT HelperWindow::winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto winObj = reinterpret_cast<HelperWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (winObj != nullptr)
    {
        return winObj->winMsg(hwnd, msg, wParam, lParam);
    }
    else {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
LRESULT HelperWindow::winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_TRAY_CREATE)
    {
        auto trayData = (TrayData*)lParam;
        createTray(trayData);
        return true;
    }
    else if (msg == WM_TRAY) {
        auto trayId = (int)wParam;
        auto winId = trayWinId[trayId];
        if (lParam == WM_RBUTTONDOWN) {
            //JsonResult result(winId, "tray", std::to_string(trayId));
            //result.addString("type", "rightBtnDown");
            //result.returnBack();
            SetForegroundWindow(hwnd);
            POINT pt;
            GetCursorPos(&pt);
            HMENU menu = CreatePopupMenu();
            AppendMenu(menu, MF_STRING, 1000000, L"测试数据测试");
            TrackPopupMenu(menu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);



            //if (menus.contains(trayId)) {
            //    auto menu = menus[trayId];
            //    SetForegroundWindow(hwnd);
            //    TrackPopupMenu(menu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);
            //}
        }
        else if (lParam == WM_LBUTTONDOWN) {
            POINT pt;
            GetCursorPos(&pt);
            //SetForegroundWindow(hwnd);
            //JsonResult result(winId, "tray", std::to_string(wParam));
            //result.addString("type", "leftBtnDown");
            //result.returnBack();
        }
    }
    else if (msg == WM_COMMAND) {
        auto trayId = (int)wParam;
        auto winId = trayWinId[trayId];
        //JsonResult result(winId, "tray", std::to_string(trayId));
        //result.returnBack();
    }
}
void HelperWindow::startCreateTray(TrayData* trayData)
{
    while (!hwndReady) //窗口可能还没创建成功
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(60));//避免忙等待
    }
    PostMessage(hwnd, WM_TRAY_CREATE, 0, (LPARAM)trayData);
}
