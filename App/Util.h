#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <Windows.h>
#include <WebView2.h>
#include <wrl.h>
#include <wil/com.h>
#include <gdiplus.h>
#include "../Lib/rapidjson/document.h"

class Util
{
public:
	static std::string readFile(const std::wstring& filePath);
	static std::wstring convertToWStr(const char* str);
	static std::string convertToStr(const std::wstring& wstr);
};

