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
class App
{
public:
	App();
	~App();
	static App* get();
	static void init();
public:
	ICoreWebView2Environment* env;
	rapidjson::Document d;
	std::string appId;
private:
	void start();
	bool checkRuntime();
	bool checkRegKey(const HKEY& key, const std::wstring& subKey);
	std::filesystem::path ensureAppFolder();
private:
	HRESULT envReady(HRESULT result, ICoreWebView2Environment* env);
};

