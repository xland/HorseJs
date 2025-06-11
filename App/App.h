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
	static App* get();
	static BrowserWindow* getWindow(const int& id);
	static void closeAllWindowAsync();
	static bool hasWindow();
	static void addWindow(std::unique_ptr<BrowserWindow> win);
	static void init(HINSTANCE hInstance);
public:
	bool quitWhenAllWindowClosed{ true };
	std::string appId;
	wil::com_ptr<ICoreWebView2Environment> env;
	HINSTANCE hInstance;
private:
	void loadConfig();
	void createEnv();
	void start();
	void checkRuntime();
	bool checkRegKey(const HKEY& key, const std::wstring& subKey);
	void ensureAppFolder();
	HRESULT envReady(HRESULT result, ICoreWebView2Environment* env);
private:
	std::filesystem::path appDir;
};

