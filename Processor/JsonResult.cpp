#include <pch.h>
#include "../App/App.h"
#include "JsonResult.h"
#include "../App/BrowserWindow.h"

JsonResult::JsonResult(const int& winId,const std::string& className,const std::string& eventName):
    winId{ winId }
{
    addString("className", className);
    addString("eventName", eventName);
}

JsonResult::~JsonResult()
{
}

void JsonResult::addErr(const std::string& value)
{
    ok = false;
    addBool("ok", false);
    addString("err", value);
}
void JsonResult::returnBackThread() {
    auto win = App::get()->getWindow(winId);
    PostMessage(win->hwnd, WM_THREAD_RESULT, 0, (LPARAM)this);
}
void JsonResult::returnBack()
{
    if (cancel) return;
    if (ok) {
        addBool("ok", true);
    }
    std::wstring jsonStr = parse();
    auto win = App::get()->getWindow(winId);
    win->webview->PostWebMessageAsJson(jsonStr.data());
}

void JsonResult::returnBackSharedBuffer(ICoreWebView2SharedBuffer* sharedBuffer) //todo 到不了这个方法
{
    if (ok) {
        addBool("ok", true);
    }
    std::wstring jsonStr = parse();
    auto win = App::get()->getWindow(winId);
    auto webview17 = win->webview.try_query<ICoreWebView2_17>();
    webview17->PostSharedBufferToScript(sharedBuffer, COREWEBVIEW2_SHARED_BUFFER_ACCESS_READ_ONLY, jsonStr.data());
}

BrowserWindow* JsonResult::getTar()
{
    if (tarId < 0) {
        return App::get()->getWindow(winId);
    }
    else {
        return App::get()->getWindow(tarId);
    }
}

BrowserWindow* JsonResult::getWin()
{
    return App::get()->getWindow(winId);
}
