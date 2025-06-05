#include <pch.h>
#include "../App/App.h"
#include "JsonResult.h"
#include "../App/BrowserWindow.h"

JsonResult::JsonResult(BrowserWindow* win,BrowserWindow* tar, std::string& className, std::string& eventName) :
	win{ win }, tar{ tar }, className{ className }, eventName{ eventName }
{
}

JsonResult::~JsonResult()
{
}

JsonResult* JsonResult::create(BrowserWindow* win, BrowserWindow* tar, std::string& className, std::string& eventName)
{
    auto result = new JsonResult(win,tar,className,eventName);
    return result;
}

void JsonResult::addErr(const std::string& value)
{
    ok = false;
    addBool("ok", false);
    addString("className", className);
    addString("eventName", eventName);
    addString("err", value);
    returnBack();
}
void back() {

}
void JsonResult::returnBackThread() {
    //auto id = GetCurrentThreadId();
    //DWORD winThreadId = GetWindowThreadProcessId(win->hwnd, NULL);
    //if (id != winThreadId) {
    //}
    PostMessage(win->hwnd, WM_THREADRESULT, 0, (LPARAM)this);
}
void JsonResult::returnBack(bool delSelf)
{
    if (ok) {
        addBool("ok", true);
    }
    addString("className", className);
    addString("eventName", eventName);
    std::wstring jsonStr = parse();
    win->webview->PostWebMessageAsJson(jsonStr.data());
    if(delSelf) delete this;
}

void JsonResult::returnBackSharedBuffer() //todo 到不了这个方法
{
    if (ok) {
        addBool("ok", true);
    }
    addString("className", className);
    addString("eventName", eventName);
    std::wstring jsonStr = parse();
    auto webview17 = win->webview.try_query<ICoreWebView2_17>();
    webview17->PostSharedBufferToScript(this->sharedBuffer, COREWEBVIEW2_SHARED_BUFFER_ACCESS_READ_ONLY, jsonStr.data());
}
