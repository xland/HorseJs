#pragma once
#include <windows.h>
#include <DispatcherQueue.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <wil/com.h>
#include <WebView2.h>
#include "../App/Util.h"
#include "../App/JsonParsor.h"

using namespace Microsoft;
using namespace winrt::Windows;
class Page;
class BrowserWindowConfig;
class MsgProcessor;
class BrowserWindow
{
public:
	BrowserWindow(const rapidjson::Value& winConfig);
	~BrowserWindow();
	bool load(const rapidjson::Value& pageConfig);
public:
	void show(const rapidjson::Value& params, JsonParsor& result);
	void hide(const rapidjson::Value& params, JsonParsor& result);
	void maximize(const rapidjson::Value& params, JsonParsor& result);
	void minimize(const rapidjson::Value& params, JsonParsor& result);
	void restore(const rapidjson::Value& params, JsonParsor& result);
	void flash(const rapidjson::Value& params, JsonParsor& result);
	void close(const rapidjson::Value& params, JsonParsor& result);
	void destroy(const rapidjson::Value& params, JsonParsor& result);
	void startDrag(const rapidjson::Value& params, JsonParsor& result);
	void openWindow(const rapidjson::Value& params, JsonParsor& result);
	void setResizable(const rapidjson::Value& params, JsonParsor& result);
	void resize(const rapidjson::Value& params, JsonParsor& result);
	void addEventListener(const rapidjson::Value& params, JsonParsor& result);
	void removeEventListener(const rapidjson::Value& params, JsonParsor& result);
public:
	wil::com_ptr<ICoreWebView2Controller> ctrl;
	wil::com_ptr<ICoreWebView2CompositionController> ctrlComp;

	System::DispatcherQueueController m_dispatcherQueueController{ nullptr };
	UI::Composition::Compositor m_compositor{ nullptr };
	UI::Composition::Desktop::DesktopWindowTarget m_target{ nullptr };
	UI::Composition::ContainerVisual m_rootVisual{ nullptr };
	UI::Composition::ContainerVisual m_webViewVisual{ nullptr };

	HWND hwnd;
	wil::unique_hicon favicon;
	std::unique_ptr<BrowserWindowConfig> config;
	std::unordered_map<std::string, bool> eventFlag;
	std::unique_ptr<Page> page;
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
	bool isMouseTracking{ false }, framelessResizable{true};
};

