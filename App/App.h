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
#include "../Lib/rapidjson/document.h"

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
	ICoreWebView2Environment* env;
	rapidjson::Document d;
	std::string appId;
private:
	std::vector<BrowserWindow*> windows;
	bool quitWhenAllWindowClosed{ true };
private:
	void start();
	bool checkRuntime();
	bool checkRegKey(const HKEY& key, const std::wstring& subKey);
	std::filesystem::path ensureAppFolder();
	HRESULT envReady(HRESULT result, ICoreWebView2Environment* env);
};

