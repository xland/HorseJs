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

	template<typename T>
    static std::string getJsonStr(const std::map<std::string, T>& m) {
        std::ostringstream oss;
        oss << "{";
        bool first = true;

        for (const auto& [key, value] : m) {
            if (!first) oss << ",";
            first = false;

            oss << "\"" << key << "\":";

            if constexpr (std::is_same<T, std::string>::value) {
                if (value == "[]") {
                    oss << std::string(value);
                }
                else {
                    oss << "\"" << std::string(value) << "\"";
                }                
            }
            else if constexpr (std::is_arithmetic<T>::value) {
                oss << value; // 直接输出数字类型
            }
            else {
                static_assert(sizeof(T) == 0, "Unsupported type for JSON serialization");
            }
        }

        oss << "}";
        return oss.str();
    }
};

