#pragma once
#include <windows.h>
#include <DispatcherQueue.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
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
	wil::com_ptr<ICoreWebView2CompositionController> ctrlComp;
	winrt::Windows::System::DispatcherQueueController m_dispatcherQueueController{ nullptr };
	winrt::Windows::UI::Composition::Compositor m_compositor{ nullptr };
	winrt::Windows::UI::Composition::Desktop::DesktopWindowTarget m_target{ nullptr };
	winrt::Windows::UI::Composition::ContainerVisual m_rootVisual{ nullptr };
	winrt::Windows::UI::Composition::ContainerVisual m_webViewVisual{ nullptr };

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
	HRESULT ctrlReady(HRESULT result, ICoreWebView2CompositionController* ctrl);
private:
	bool isMouseTracking{ false };
};

