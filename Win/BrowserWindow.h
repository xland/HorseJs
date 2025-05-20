#pragma once
#include <windows.h>
#include <wil/com.h>
#include <WebView2.h>
#include "../App/Util.h"

class Page;
class BrowserWindowConfig;
class BrowserWindow
{
public:
	BrowserWindow(rapidjson::Value& winConfig);
	~BrowserWindow();
	bool load(rapidjson::Value& pageConfig);
	void call(rapidjson::Document& jsonDoc);
public:
	wil::com_ptr<ICoreWebView2Controller> ctrl;
	HWND hwnd;
	wil::unique_hicon favicon;
	std::unique_ptr<BrowserWindowConfig> config;
protected:
private:
	void initWindow();
	WNDCLASSEX* regWinClass();
	void show();
	static LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	HRESULT pageCtrlReady(HRESULT result, ICoreWebView2Controller* ctrl);
	
private:
	std::unique_ptr<Page> page;
};

