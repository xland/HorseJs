#pragma once
#include <windows.h>
#include <Shlobj.h>
#include <shellapi.h>
#include <fstream>
#include <filesystem>
#include <wrl.h>
#include <wil/com.h>
#include <WebView2.h>
#include "Util.h"
#include "AppConfig.h"
#include "../Lib/rapidjson/document.h"
#include "JsonParsor.h"

class BrowserWindow;
class App
{
public:
	App();
	~App();
	void onWindowDestroy(BrowserWindow* win);
	static App* get();
	static void init();
public:
	std::unique_ptr<AppConfig> config;
	ICoreWebView2Environment* env;
private:
	std::vector<std::unique_ptr<BrowserWindow>> windows;
private:
	void start();
	bool checkRuntime();
	bool checkRegKey(const HKEY& key, const std::wstring& subKey);
	std::filesystem::path ensureAppFolder();
	HRESULT envReady(HRESULT result, ICoreWebView2Environment* env);
};

