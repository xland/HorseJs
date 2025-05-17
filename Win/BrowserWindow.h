#pragma once
#include <windows.h>
#include <wil/com.h>
#include <WebView2.h>
#include "../App/Util.h"

class Page;
class BrowserWindow
{
public:
	BrowserWindow(const int& x, const int& y, const int& w, const int& h,
		const bool& visible, const bool& frame, const bool& shadow, 
		const std::wstring& title);
	~BrowserWindow();
	static BrowserWindow* create(const rapidjson::Value& config);
public:
	wil::com_ptr<ICoreWebView2Controller> ctrl;
	HWND hwnd;
	std::wstring title;
	wil::unique_hicon favicon;
protected:
private:
	void initWindow();
	WNDCLASSEX* regWinClass();
	void show();
	bool load(const rapidjson::Value& config);
	static LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	HRESULT pageCtrlReady(HRESULT result, ICoreWebView2Controller* ctrl);
	
private:
	int x, y, w, h;
	bool visible,frame,shadow;
	std::unique_ptr<Page> page;
};

