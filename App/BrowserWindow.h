#pragma once
#include <pch.h>
#include "../App/Util.h"
#include "../Processor/JsonResult.h"


class JsonResult;
class BrowserWindow
{
public:
	BrowserWindow(const rapidjson::Value& winConfig,const int& pid=-1);
	~BrowserWindow();
	bool load();
public:
	int id{ 0 },pid{-1}, x{100}, y{100}, w{1000}, h{800};
	int minWidth{ 0 }, minHeight{ 0 }, maxWidth{ std::numeric_limits<int>::max() }, maxHeight{ std::numeric_limits<int>::max() };
	bool maximize{ false }, visible{ true }, frame{ true }, shadow{ true }, skipTaskbar{ false };
	bool maximizable{ true }, resizable{ true }, minimizable{ true }, alwaysOnTop{ false };  //todo closable
	std::wstring title{ L"HorseJs" };
public:
	bool framelessResizable{ true };
	HWND hwnd;
	wil::unique_hicon favicon;
	//  set不重复，即使一个窗口注册了多个相同的事件，
	//  只要一个result returnBack之后页面会emit多次回调的
	std::unordered_map<std::string, std::set<int>> events;
	wil::com_ptr<ICoreWebView2> webview;
	std::vector<NOTIFYICONDATA*> trays;
	std::map<int, HMENU> trayMenus;
	wil::com_ptr<ICoreWebView2CompositionController> ctrlComp;
protected:
private:
	HRESULT ctrlReady(HRESULT result, ICoreWebView2CompositionController* ctrl);
	void setWindowStyle(long& exStyle, long& style);
	void bindCompCtrlToHwnd();
	std::wstring& getWinClsName();
	void initWindow();
private:
	void sizePosChanged(WINDOWPOS* winPos);
	void stateChanged(const int& state);
	void closing();
	void setMinMaxInfo(MINMAXINFO* mmi);
	int hittest(const POINT& pt);
	static LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	HRESULT cursorChange(ICoreWebView2CompositionController*, IUnknown*);
private:
	void configSize(const rapidjson::Value& config);
	void configPos(const rapidjson::Value& config);
	void configPage(const rapidjson::Value& config);
	void configOther(const rapidjson::Value& config);
private:
	void routeMsgToPage(UINT msg, WPARAM wParam, LPARAM lParam);
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
	HRESULT resRequested(ICoreWebView2* webview, ICoreWebView2WebResourceRequestedEventArgs* args);
	void loadResource();
	void loadPage();
	std::string getMsgStr(ICoreWebView2WebMessageReceivedEventArgs* args);
private:
	bool isMouseTracking{ false };
	wil::com_ptr<ICoreWebView2Controller> ctrl;
	UI::Composition::Desktop::DesktopWindowTarget m_target{ nullptr };
	UI::Composition::ContainerVisual m_rootVisual{ nullptr };
	UI::Composition::ContainerVisual m_webViewVisual{ nullptr };
private:
	bool scriptDialogEnable{ true }, webMessageEnable{ true }, scriptEnable{ true }, contextMenuEnable{ true };
	std::wstring url{L"index.html"};
};

