#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <stack>
#include <fstream>
#include <filesystem>
#include <functional>
#include <shared_mutex>
#include <regex>

#include <windows.h>
#include <winuser.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <wincodec.h>
#include <comdef.h>
#include <shellscalingapi.h>

#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>
#include <wrl.h>
#include <wil/com.h>
#include <DispatcherQueue.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.UI.Composition.Desktop.h>


#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

using namespace Microsoft;
using namespace winrt::Windows;

#define WM_SECOND_INSTANCE_NOTIFY (WM_USER + 100)
#define WM_THREAD_RESULT (WM_USER + 101)
#define WM_TRAY (WM_USER + 102)
#define WM_TRAY_CREATE (WM_USER + 103)
