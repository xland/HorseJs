#pragma once
#include <windows.h>
#include <wil/com.h>
#include "../App/Util.h"

class BrowserWindow;
class Page
{
public:
	Page(BrowserWindow* win);
	~Page();
	void init(const rapidjson::Value& config);
	void load();
public:
	wil::com_ptr<ICoreWebView2> webview;
private:
	HRESULT navigateStart(ICoreWebView2* webview, ICoreWebView2NavigationStartingEventArgs* args);
	HRESULT navigateEnd(ICoreWebView2* webview, ICoreWebView2NavigationCompletedEventArgs* args);
	HRESULT titleChange(ICoreWebView2* sender, IUnknown* args);
	HRESULT statusChange(ICoreWebView2* sender, IUnknown* args);
	HRESULT faviconChange(ICoreWebView2* sender, IUnknown* args);
	HRESULT newWindowRequeste(ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args);
	HRESULT msgReceive(ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args);
	void loadResource();
private:
	BrowserWindow* win;
	bool areDefaultScriptDialogsEnabled{ true };
	bool isWebMessageEnabled{ true };
	bool isScriptEnabled{ true };
};

