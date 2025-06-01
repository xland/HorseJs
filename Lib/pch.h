#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <map>
#include <fstream>
#include <filesystem>
#include <functional>
#include <regex>

#include <windows.h>
#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>
#include <wrl.h>
#include <wil/com.h>
#include <Shlobj.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>


#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#define WM_THREADRESULT (WM_USER + 100)
