#include <wil/com.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>

#include "Page.h"
#include "BrowserWindow.h"
#include "JsBridge.h"
#include "../App/App.h"

using namespace Microsoft;

Page::Page(BrowserWindow* win) : win{ win }
{
}

Page::~Page()
{
}

void Page::init(const rapidjson::Value& config)
{
	if (config.HasMember("areDefaultScriptDialogsEnabled") && config["areDefaultScriptDialogsEnabled"].IsBool())
	{
		areDefaultScriptDialogsEnabled = config["areDefaultScriptDialogsEnabled"].GetBool();
	}
	if (config.HasMember("isWebMessageEnabled") && config["isWebMessageEnabled"].IsBool())
	{
		isWebMessageEnabled = config["isWebMessageEnabled"].GetBool();
	}
	if (config.HasMember("isScriptEnabled") && config["isScriptEnabled"].IsBool())
	{
		isScriptEnabled = config["isScriptEnabled"].GetBool();
	}
}

void Page::load()
{
    HRESULT hr = win->ctrl->get_CoreWebView2(&webview);
    auto app = App::get();
    auto appId = Util::convertToWStr(app->appId.data());
    auto webView3 = webview.try_query<ICoreWebView2_3>();
    webView3->SetVirtualHostNameToFolderMapping(appId.data(),L"UI",COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_ALLOW);

    wil::com_ptr<ICoreWebView2Settings> settings;
    webview->get_Settings(&settings);
    settings->put_IsScriptEnabled(isScriptEnabled);
    settings->put_AreDefaultScriptDialogsEnabled(areDefaultScriptDialogsEnabled);
    settings->put_IsWebMessageEnabled(isWebMessageEnabled);


    auto navigateCB = WRL::Callback<ICoreWebView2NavigationStartingEventHandler>(this, &Page::navigationStarting);
    EventRegistrationToken navigateToken;
    webview->add_NavigationStarting(navigateCB.Get(), &navigateToken);

    auto titleChangedCB = WRL::Callback<ICoreWebView2DocumentTitleChangedEventHandler>(this, &Page::titleChanged);
    EventRegistrationToken titleToken;
    hr = webview->add_DocumentTitleChanged(titleChangedCB.Get(), &titleToken);

    auto statusChangeCB = WRL::Callback<ICoreWebView2StatusBarTextChangedEventHandler>(this, &Page::statusChanged);
    EventRegistrationToken statusToken;
    auto webView12 = webview.try_query<ICoreWebView2_12>();
    hr = webView12->add_StatusBarTextChanged(statusChangeCB.Get(), &statusToken);


    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken_;
    Gdiplus::GdiplusStartup(&gdiplusToken_, &gdiplusStartupInput, NULL);
    auto webView15 = webview.try_query<ICoreWebView2_15>();
    EventRegistrationToken faviconToken;
    auto faviconChangeCB = WRL::Callback<ICoreWebView2FaviconChangedEventHandler>(this, &Page::faviconChange);
    hr = webView15->add_FaviconChanged(faviconChangeCB.Get(), &faviconToken);

    auto newWindowCB = WRL::Callback<ICoreWebView2NewWindowRequestedEventHandler>(this, &Page::newWindowRequested);
    EventRegistrationToken newWindowToken;
    hr = webView15->add_NewWindowRequested(newWindowCB.Get(), &newWindowToken);



    auto jsBridge = winrt::make<JsBridge>();
    VARIANT hostObject;
    VariantInit(&hostObject);
    hostObject.vt = VT_UNKNOWN;
    IUnknown* pUnknown = reinterpret_cast<IUnknown*>(winrt::get_abi(jsBridge));
    pUnknown->AddRef();
    hostObject.punkVal = pUnknown;
    hr = webview->AddHostObjectToScript(L"hostObj", &hostObject);
    if (FAILED(hr)) {
        auto a = 1;
    }

    webview->Navigate(L"https://HorseJs/index.html");
}


HRESULT Page::navigationStarting(ICoreWebView2* webview, ICoreWebView2NavigationStartingEventArgs* args)
{
    return S_OK;
}

HRESULT Page::titleChanged(ICoreWebView2* sender, IUnknown* args)
{
    LPWSTR titleData;
    HRESULT hr = webview->get_DocumentTitle(&titleData);
    win->title = titleData;
    SetWindowText(win->hwnd, win->title.data());
    CoTaskMemFree(titleData);
    return S_OK;
}

HRESULT Page::statusChanged(ICoreWebView2* sender, IUnknown* args)
{
    LPWSTR statusData;
    auto m_webView2_12 = webview.try_query<ICoreWebView2_12>();
    HRESULT hr = m_webView2_12->get_StatusBarText(&statusData);
    CoTaskMemFree(statusData);
    return S_OK;
}

HRESULT Page::faviconChange(ICoreWebView2* sender, IUnknown* args)
{
    auto webView15 = webview.try_query<ICoreWebView2_15>();
    LPWSTR urlData;
    webView15->get_FaviconUri(&urlData);
    std::wstring url = urlData;
    CoTaskMemFree(urlData);
    webView15->GetFavicon(COREWEBVIEW2_FAVICON_IMAGE_FORMAT_PNG,
        WRL::Callback<ICoreWebView2GetFaviconCompletedHandler>([this, url](HRESULT errorCode, IStream* iconStream)
            {
                Gdiplus::Bitmap iconBitmap(iconStream);
                wil::unique_hicon icon;
                auto hr = iconBitmap.GetHICON(&icon);
                if (hr == Gdiplus::Status::Ok)
                {
                    win->favicon = std::move(icon);
                    SendMessage(win->hwnd, WM_SETICON, ICON_SMALL, (LPARAM)win->favicon.get());
                }
                else
                {
                    SendMessage(win->hwnd, WM_SETICON, ICON_SMALL, (LPARAM)IDC_NO);
                }
                return S_OK;
            }).Get());
    return S_OK;
}

HRESULT Page::newWindowRequested(ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args)
{
    //if (!m_shouldHandleNewWindowRequest)
    //{
    //    args->put_Handled(FALSE);
    //    return S_OK;
    //}
    //wil::com_ptr<ICoreWebView2NewWindowRequestedEventArgs> args_as_comptr = args;
    //auto args3 = args_as_comptr.try_query<ICoreWebView2NewWindowRequestedEventArgs3>();
    //if (args3)
    //{
    //    wil::com_ptr<ICoreWebView2FrameInfo> frame_info;
    //    CHECK_FAILURE(args3->get_OriginalSourceFrameInfo(&frame_info));
    //    wil::unique_cotaskmem_string source;
    //    CHECK_FAILURE(frame_info->get_Source(&source));
    //    // The host can decide how to open based on source frame info,
    //    // such as URI.
    //    static const wchar_t* browser_launching_domain = L"www.example.com";
    //    wil::unique_bstr source_domain = GetDomainOfUri(source.get());
    //    const wchar_t* source_domain_as_wchar = source_domain.get();
    //    if (source_domain_as_wchar &&
    //        wcscmp(browser_launching_domain, source_domain_as_wchar) == 0)
    //    {
    //        // Open the URI in the default browser.
    //        wil::unique_cotaskmem_string target_uri;
    //        CHECK_FAILURE(args->get_Uri(&target_uri));
    //        ShellExecute(
    //            nullptr, L"open", target_uri.get(), nullptr, nullptr,
    //            SW_SHOWNORMAL);
    //        CHECK_FAILURE(args->put_Handled(TRUE));
    //        return S_OK;
    //    }
    //}

    //wil::com_ptr<ICoreWebView2Deferral> deferral;
    //CHECK_FAILURE(args->GetDeferral(&deferral));
    //AppWindow* newAppWindow;

    //wil::com_ptr<ICoreWebView2WindowFeatures> windowFeatures;
    //CHECK_FAILURE(args->get_WindowFeatures(&windowFeatures));

    //RECT windowRect = { 0 };
    //UINT32 left = 0;
    //UINT32 top = 0;
    //UINT32 height = 0;
    //UINT32 width = 0;
    //BOOL shouldHaveToolbar = true;

    //BOOL hasPosition = FALSE;
    //BOOL hasSize = FALSE;
    //CHECK_FAILURE(windowFeatures->get_HasPosition(&hasPosition));
    //CHECK_FAILURE(windowFeatures->get_HasSize(&hasSize));

    //bool useDefaultWindow = true;

    //if (!!hasPosition && !!hasSize)
    //{
    //    CHECK_FAILURE(windowFeatures->get_Left(&left));
    //    CHECK_FAILURE(windowFeatures->get_Top(&top));
    //    CHECK_FAILURE(windowFeatures->get_Height(&height));
    //    CHECK_FAILURE(windowFeatures->get_Width(&width));
    //    useDefaultWindow = false;
    //}
    //CHECK_FAILURE(windowFeatures->get_ShouldDisplayToolbar(&shouldHaveToolbar));

    //windowRect.left = left;
    //windowRect.right = left + (width < s_minNewWindowSize ? s_minNewWindowSize : width);
    //windowRect.top = top;
    //windowRect.bottom = top + (height < s_minNewWindowSize ? s_minNewWindowSize : height);

    //// passing "none" as uri as its a noinitialnavigation
    //if (!useDefaultWindow)
    //{
    //    newAppWindow = new AppWindow(m_creationModeId, GetWebViewOption(), L"none", m_userDataFolder, false,
    //        nullptr, true, windowRect, !!shouldHaveToolbar);
    //}
    //else
    //{
    //    newAppWindow = new AppWindow(m_creationModeId, GetWebViewOption(), L"none");
    //}
    //newAppWindow->m_isPopupWindow = true;
    //newAppWindow->m_onWebViewFirstInitialized = [args, deferral, newAppWindow]()
    //    {
    //        CHECK_FAILURE(args->put_NewWindow(newAppWindow->m_webView.get()));
    //        CHECK_FAILURE(args->put_Handled(TRUE));
    //        CHECK_FAILURE(deferral->Complete());
    //    };
    return S_OK;
}
