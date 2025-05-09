#include "Page.h"

Page::Page(wil::com_ptr<ICoreWebView2> webview, WinBase* win) :webview{ webview }, win{ win }
{
	wil::com_ptr<ICoreWebView2Settings> settings;
	webview->get_Settings(&settings);
	settings->put_IsScriptEnabled(TRUE);
	settings->put_AreDefaultScriptDialogsEnabled(TRUE);
	settings->put_IsWebMessageEnabled(TRUE);
}

Page::~Page()
{
}
