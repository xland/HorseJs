#include <wil/com.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>

#include "Page.h"
#include "BrowserWindow.h"
#include "../App/Res.h"
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
    auto appId = Util::convertToWStr(app->config->appId.data());
    auto webView3 = webview.try_query<ICoreWebView2_3>();
    webView3->SetVirtualHostNameToFolderMapping(appId.data(),L"UI",COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_ALLOW);

    wil::com_ptr<ICoreWebView2Settings> settings;
    webview->get_Settings(&settings);
    settings->put_IsScriptEnabled(isScriptEnabled);
    settings->put_AreDefaultScriptDialogsEnabled(areDefaultScriptDialogsEnabled);
    settings->put_IsWebMessageEnabled(isWebMessageEnabled);


    auto navigateStartCB = WRL::Callback<ICoreWebView2NavigationStartingEventHandler>(this, &Page::navigateStart);
    EventRegistrationToken navigateStartToken;
    webview->add_NavigationStarting(navigateStartCB.Get(), &navigateStartToken);

    auto navigateEndCB = WRL::Callback<ICoreWebView2NavigationCompletedEventHandler>(this, &Page::navigateEnd);
    EventRegistrationToken navigateEndToken;
    webview->add_NavigationCompleted(navigateEndCB.Get(),&navigateEndToken);

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

    EventRegistrationToken msgReceivedToken;
    auto msgReceivedCB = WRL::Callback<ICoreWebView2WebMessageReceivedEventHandler>(this, &Page::msgReceived);
    webview->add_WebMessageReceived(msgReceivedCB.Get(), &msgReceivedToken);

    loadResource();    

    webview->Navigate(L"https://HorseJs/index.html");
}

void Page::loadResource()
{
    HMODULE hModule = GetModuleHandleW(nullptr);
    HRSRC hResource = FindResourceW(hModule, MAKEINTRESOURCEW(IDR_JS), RT_RCDATA);
    if (!hResource) throw std::runtime_error("Failed to find resource");
    HGLOBAL hLoadedResource = LoadResource(hModule, hResource);
    if (!hLoadedResource) throw std::runtime_error("Failed to load resource");
    DWORD resourceSize = SizeofResource(hModule, hResource);
    if (resourceSize == 0) throw std::runtime_error("Resource size is zero");
    void* pResourceData = LockResource(hLoadedResource);
    if (!pResourceData) throw std::runtime_error("Failed to lock resource");
    std::wstring script = Util::convertToWStr((char*)pResourceData);
    webview->AddScriptToExecuteOnDocumentCreated(script.data(), nullptr);
}

HRESULT Page::navigateStart(ICoreWebView2* webview, ICoreWebView2NavigationStartingEventArgs* args)
{
    return S_OK;
}

HRESULT Page::navigateEnd(ICoreWebView2* webview, ICoreWebView2NavigationCompletedEventArgs* args)
{
    //JsonParsor parsor;
    //parsor.addString("eventName", "window_close");
    //rapidjson::Value items(rapidjson::kArrayType);
    //parsor.addValue("param", std::move(items));
    //std::wstring jsonStr = parsor.parse();
    //webview->PostWebMessageAsJson(jsonStr.data());
    return S_OK;
}

HRESULT Page::titleChanged(ICoreWebView2* sender, IUnknown* args)
{
    //wil::unique_cotaskmem_string titleData;
    //HRESULT hr = webview->get_DocumentTitle(&titleData);
    //win->title = titleData.get();
    //SetWindowText(win->hwnd, win->title.data());
    return S_OK;
}

HRESULT Page::statusChanged(ICoreWebView2* sender, IUnknown* args)
{
    //wil::unique_cotaskmem_string statusData;
    //auto m_webView2_12 = webview.try_query<ICoreWebView2_12>();
    //HRESULT hr = m_webView2_12->get_StatusBarText(&statusData);
    return S_OK;
}

HRESULT Page::faviconChange(ICoreWebView2* sender, IUnknown* args)
{
    //auto webView15 = webview.try_query<ICoreWebView2_15>();
    //wil::unique_cotaskmem_string urlData;
    //webView15->get_FaviconUri(&urlData);
    //webView15->GetFavicon(COREWEBVIEW2_FAVICON_IMAGE_FORMAT_PNG,
    //    WRL::Callback<ICoreWebView2GetFaviconCompletedHandler>([this](HRESULT errorCode, IStream* iconStream)
    //        {
    //            Gdiplus::Bitmap iconBitmap(iconStream);
    //            wil::unique_hicon icon;
    //            auto hr = iconBitmap.GetHICON(&icon);
    //            if (hr == Gdiplus::Status::Ok)
    //            {
    //                win->favicon = std::move(icon);
    //                SendMessage(win->hwnd, WM_SETICON, ICON_SMALL, (LPARAM)win->favicon.get());
    //            }
    //            else
    //            {
    //                SendMessage(win->hwnd, WM_SETICON, ICON_SMALL, (LPARAM)IDC_NO);
    //            }
    //            return S_OK;
    //        }).Get());
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

HRESULT Page::msgReceived(ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args)
{
    wil::unique_cotaskmem_string messageRaw;
    args->get_WebMessageAsJson(&messageRaw);
    std::wstring message = messageRaw.get();
    auto str = Util::convertToStr(message);

    rapidjson::Document jsonDoc;
    jsonDoc.Parse(str.data());

    if (jsonDoc.HasMember("msgType") && jsonDoc["msgType"].IsInt()) {
        auto eventId = jsonDoc["msgType"].GetInt();
        auto a = 1;
    }



    if (jsonDoc.HasMember("eventId") && jsonDoc["eventId"].IsString())
    {
        auto eventId = jsonDoc["eventId"].GetString();

        JsonParsor parsor;
        parsor.addString("eventId", eventId);
        rapidjson::Value items(rapidjson::kArrayType);
        rapidjson::Value number1(42); // 第一个数字
        rapidjson::Value number2(3.14); // 第二个数字（支持浮点数）
        items.PushBack(number1, parsor.getAllocator());
        items.PushBack(number2, parsor.getAllocator());
        parsor.addValue("param", std::move(items));
        std::wstring jsonStr = parsor.parse();
        webview->PostWebMessageAsJson(jsonStr.data());
    }




    return S_OK;
}
