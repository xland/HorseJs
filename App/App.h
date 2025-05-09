#pragma once
#include <windows.h>
#include <Shlobj.h>
#include <shellapi.h>
#include <fstream>
#include <filesystem>
#include <functional>
#include <wrl.h>
#include <wil/com.h>
#include <WebView2.h>
#include "Util.h"
#include "../Lib/rapidjson/document.h"
class App
{
public:
	static void init();
private:
	bool checkRuntime();
	bool checkRegKey(const HKEY& key, const std::wstring& subKey);
private:
	ICoreWebView2Environment* env;
};

