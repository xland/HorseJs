#include <pch.h>
#include "App.h"
#include "BrowserWindow.h"

LRESULT BrowserWindow::winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_SECOND_INSTANCE_NOTIFY) {
        App::get()->onSecondInstance();
    }
    else if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST)
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
    else if (msg == WM_SETCURSOR) {
        if (LOWORD(lParam) != HTCLIENT) goto sysProcess;
        if (!ctrlComp) goto sysProcess;
        HCURSOR cursor = nullptr;
        auto hr = ctrlComp->get_Cursor(&cursor);
        if (FAILED(hr)) return false;
        if (!cursor) goto sysProcess;
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
        stateChanged((int)wParam);
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
        JsonResult result(id, "os", std::to_string(wParam));
        result.returnBack();
    }
    else if (msg == WM_TRAY) {
        if (lParam == WM_RBUTTONDOWN) {
            JsonResult result(id, "os", std::to_string(wParam));
            result.addString("type", "rightBtnDown");
            result.returnBack();
            POINT pt;
            GetCursorPos(&pt);
            if (trayMenus.contains((int)wParam)) {
                SetForegroundWindow(hwnd);
                TrackPopupMenuEx(trayMenus[(int)wParam], TPM_RIGHTBUTTON, pt.x, pt.y, hwnd, nullptr);
            }
        }
        else if (lParam == WM_LBUTTONDOWN) {
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            JsonResult result(id, "os", std::to_string(wParam));
            result.addString("type", "leftBtnDown");
            result.returnBack();
        }
    }
    else if (msg == WM_WTSSESSION_CHANGE) {
        if (wParam == WTS_SESSION_LOCK) {
            JsonResult r(id, "os", "osLock");
            r.addString("type", "lock");
            r.returnBack();
        }
        else if (wParam == WTS_SESSION_UNLOCK) {
            JsonResult r(id, "os", "osLock");
            r.addString("type", "unlock");
            r.returnBack();
        }
        return false;
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
    for (auto& id : vec)
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
            JsonResult result(id, "win", "closing");
            result.returnBack();
        }
        return; //×èÖ¹´°¿Ú¹Ø±Õ
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
int BrowserWindow::hittest(const POINT& pt)
{
    if (!framelessResizable) return HTCLIENT;
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