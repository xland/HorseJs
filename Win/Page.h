#pragma once
#include "../App/Util.h"
class WinBase;
class Page
{
public:
	Page(wil::com_ptr<ICoreWebView2> webview, WinBase* win);
	~Page();
private:
	wil::com_ptr<ICoreWebView2> webview;
	WinBase* win;
};

