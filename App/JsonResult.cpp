#include <pch.h>
#include "App.h"
#include "JsonResult.h"
#include "../Win/BrowserWindow.h"
#include "../Win/Page.h"

JsonResult::JsonResult(BrowserWindow* win) :win{win}
{
}

JsonResult::~JsonResult()
{
}

JsonResult* JsonResult::create(BrowserWindow* win, std::string& className, std::string& eventName)
{
    auto result = new JsonResult(win);
    result->addString("className", className);
    result->addString("eventName", eventName);
    return result;
}

void JsonResult::addErr(const std::string& value)
{
    ok = false;
    addBool("ok", false);
    addString("err", value);
    returnBack();
}

void JsonResult::returnBack()
{
    auto id = GetCurrentThreadId();
    DWORD winThreadId = GetWindowThreadProcessId(win->hwnd, NULL);
    if (id == winThreadId) {
        if (ok) {
            addBool("ok", true);
        }
        std::wstring jsonStr = parse();
        win->page->webview->PostWebMessageAsJson(jsonStr.data());
        delete this;
    }
    else {
        PostMessage(win->hwnd, WM_THREADRESULT, 0, (LPARAM)this);
    }
}

void JsonResult::returnBackSharedBuffer()
{
    auto id = GetCurrentThreadId();
    DWORD winThreadId = GetWindowThreadProcessId(win->hwnd, NULL);
    if (id == winThreadId) {
        if (ok) {
            addBool("ok", true);
        }
        std::wstring jsonStr = parse();
        auto webview17 = win->page->webview.try_query<ICoreWebView2_17>();
        webview17->PostSharedBufferToScript(this->sharedBuffer, COREWEBVIEW2_SHARED_BUFFER_ACCESS_READ_ONLY, jsonStr.data());
    }
    else {
        PostMessage(win->hwnd, WM_THREADRESULT, 0, (LPARAM)this);
    }
}
