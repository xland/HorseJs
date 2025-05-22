#pragma once
#include <windows.h>
#include <wil/com.h>
#include <WebView2.h>
#include "../App/Util.h"

class Page;
class BrowserWindowConfig;
class MsgProcessor;
class BrowserWindow
{
public:
	BrowserWindow(rapidjson::Value& winConfig);
	~BrowserWindow();
	bool load(rapidjson::Value& pageConfig);
	void call(rapidjson::Document& jsonDoc);
	void resize(const int& w, const int& h);
	void regEvent(const int& eventId);
	void unregEvent(const int& eventId);
	void maximize();
	void minimize();
	void show();
	void hide();
	void restore();
public:
	wil::com_ptr<ICoreWebView2Controller> ctrl;
	HWND hwnd;
	wil::unique_hicon favicon;
	std::unique_ptr<BrowserWindowConfig> config;
	std::unique_ptr<MsgProcessor> msgProcessor;
	std::unique_ptr<Page> page;
public:
	bool closingIsReg{ false };
protected:
private:
	void initWindow();
	WNDCLASSEX* regWinClass();
	static LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	HRESULT pageCtrlReady(HRESULT result, ICoreWebView2Controller* ctrl);	
private:
	bool isMouseTracking{ false };
};

