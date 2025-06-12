#pragma once
#include <pch.h>
#include "Util.h"
#include "JsonParsor.h"

class BrowserWindow;
class App
{
public:
	App(HINSTANCE hInstance);
	~App();
	void onWindowDestroy(BrowserWindow* win);
	void onSecondInstance();
	static App* get();
	static BrowserWindow* getWindow(const int& id);
	static void closeAllWindowAsync();
	static bool hasWindow();
	static void addWindow(std::unique_ptr<BrowserWindow> win);
	static void init(HINSTANCE hInstance);
public:
	bool quitWhenAllWinClosed{ true };
	std::string appId;
	wil::com_ptr<ICoreWebView2Environment> env;
	HINSTANCE hInstance;
	std::filesystem::path appDir;
	std::unordered_map<std::string, std::set<int>> events;
	UI::Composition::Compositor compositor{nullptr};
	bool instanceWatch{ false }, instanceLock{false};
	HANDLE singleInsMutext;
private:
	void loadConfig();
	void createEnv();
	void start();
	void checkRuntime();
	bool checkRegKey(const HKEY& key, const std::wstring& subKey);
	void ensureAppFolder();
	void initUICompositor();
	void createSingleMutex();
	HRESULT envReady(HRESULT result, ICoreWebView2Environment* env);
private:
};

