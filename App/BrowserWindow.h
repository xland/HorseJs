#pragma once
#include <pch.h>
#include "../App/Util.h"
#include "../Processor/JsonResult.h"


class JsonResult;
class BrowserWindow
{
public:
	BrowserWindow(const rapidjson::Value& winConfig);
	~BrowserWindow();
	bool load();
public:
	int id{ 0 }, x{ 100 }, y{ 100 }, w{ 1000 }, h{ 800 };
	int minWidth{ 0 }, minHeight{ 0 }, maxWidth{ std::numeric_limits<int>::max() }, maxHeight{ std::numeric_limits<int>::max() };
	bool maximize{ false }, visible{ true }, frame{ true }, shadow{ true }, skipTaskbar{ false };
	bool maximizable{ true }, resizable{ true }, minimizable{ true }, alwaysOnTop{ false };  //todo closable
	std::wstring title{ L"HorseJs" };
	bool scriptDialogEnable{ true }, webMessageEnable{ true }, scriptEnable{ true }, contextMenuEnable{true};
public:
	bool framelessResizable{ true };

	wil::com_ptr<ICoreWebView2Controller> ctrl;
	wil::com_ptr<ICoreWebView2CompositionController> ctrlComp;

	System::DispatcherQueueController m_dispatcherQueueController{ nullptr };
	UI::Composition::Compositor m_compositor{ nullptr };
	UI::Composition::Desktop::DesktopWindowTarget m_target{ nullptr };
	UI::Composition::ContainerVisual m_rootVisual{ nullptr };
	UI::Composition::ContainerVisual m_webViewVisual{ nullptr };

	HWND hwnd;
	wil::unique_hicon favicon;
	std::unordered_map<std::string, std::set<int>> events;
	wil::com_ptr<ICoreWebView2> webview;
	std::vector<NOTIFYICONDATA*> trays;
	std::map<int, HMENU> trayMenus;
protected:
private:
	void setWindowStyle(long& exStyle, long& style);
	void bindCompCtrlToHwnd();
	void initWindow();
	void sizePosChanged(WINDOWPOS* winPos);
	void stateChanged(const int& state);
	void closing();
	void setMinMaxInfo(MINMAXINFO* mmi);
	void routeMsgToPage(UINT msg, WPARAM wParam, LPARAM lParam);
	std::wstring& getWinClsName();
	int hittest(const POINT& pt);
	static LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	HRESULT ctrlReady(HRESULT result, ICoreWebView2CompositionController* ctrl);
	HRESULT cursorChange(ICoreWebView2CompositionController*, IUnknown*);
private:
	void configSize(const rapidjson::Value& config);
	void configPos(const rapidjson::Value& config);
	void configPage(const rapidjson::Value& config);
	void configOther(const rapidjson::Value& config);
private:
	HRESULT navigateStart(ICoreWebView2* webview, ICoreWebView2NavigationStartingEventArgs* args);
	HRESULT navigateEnd(ICoreWebView2* webview, ICoreWebView2NavigationCompletedEventArgs* args);
	HRESULT titleChange(ICoreWebView2* sender, IUnknown* args);
	HRESULT statusChange(ICoreWebView2* sender, IUnknown* args);
	HRESULT faviconChange(ICoreWebView2* sender, IUnknown* args);
	HRESULT frameCreated(ICoreWebView2* sender, ICoreWebView2FrameCreatedEventArgs* args);
	HRESULT contextMenuRequested(ICoreWebView2* sender, ICoreWebView2ContextMenuRequestedEventArgs* args);
	HRESULT newWindowRequeste(ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args);
	HRESULT msgReceive(ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args);
	HRESULT msgReceiveIframe(ICoreWebView2Frame* webview, ICoreWebView2WebMessageReceivedEventArgs* args);
	void loadResource();
	void loadPage();
private:
	bool isMouseTracking{ false };
};

