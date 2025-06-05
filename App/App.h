#pragma once
#include <pch.h>
#include "Util.h"
#include "AppConfig.h"
#include "JsonParsor.h"

class BrowserWindow;
class App
{
public:
	App(HINSTANCE hInstance);
	~App();
	void onWindowDestroy(BrowserWindow* win);
	static App* get();
	BrowserWindow* getWindow(const int& id);
	static void init(HINSTANCE hInstance);
public:
	bool quitWhenAllWindowClosed{ true };
	std::string appId;

	wil::com_ptr<ICoreWebView2Environment> env;
	HINSTANCE hInstance;
	std::unordered_map<int, std::unique_ptr<BrowserWindow>> winMap;
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

