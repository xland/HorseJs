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
#include <memory>
#include <unordered_map>
#include <map>
#include "../Lib/rapidjson/document.h"
#include "../Lib/rapidjson/writer.h"
#include "../Lib/rapidjson/stringbuffer.h"

class Util
{
public:
	static std::string readFile(const std::wstring& filePath);
	static std::wstring convertToWStr(const char* str);
	static std::string convertToStr(const std::wstring& wstr);

	static std::wstring convertAnsiToWstring(const char* str);
	static std::string convertWstringToUtf8(const std::wstring& wstr);
	static std::string colorToHex(COLORREF color);
	static std::wstring getContentType(const std::wstring& fileName);
	static void printTime();
	static int bitmapToPngData(HBITMAP bitmap, std::vector<std::byte>& pngData,int& w,int& h);
private:
	static CLSID* getPngClsid();
};

