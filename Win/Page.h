#pragma once
#include <windows.h>
#include <wil/com.h>
#include "../App/Util.h"

class WinBase;
class Page
{
public:
	Page(WinBase* win);
	~Page();
	void init(const rapidjson::Value& config);
	void load();
private:
	HRESULT navigationStarting(ICoreWebView2* webview, ICoreWebView2NavigationStartingEventArgs* args);
	HRESULT titleChanged(ICoreWebView2* sender, IUnknown* args);
	HRESULT statusChanged(ICoreWebView2* sender, IUnknown* args);
	HRESULT faviconChange(ICoreWebView2* sender, IUnknown* args);
	HRESULT newWindowRequested(ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args);
private:
	wil::com_ptr<ICoreWebView2> webview;
	WinBase* win;
	bool areDefaultScriptDialogsEnabled{ true };
	bool isWebMessageEnabled{ true };
	bool isScriptEnabled{ true };
};

