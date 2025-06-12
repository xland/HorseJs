#include <pch.h>
#include "../App/App.h"
#include "Clipboard.h"

namespace {
    std::unique_ptr<Clipboard> clipboard;

    static std::unordered_map<std::string, void (Clipboard::*)(const rapidjson::Value&, JsonResult*)> funcs{
        {"getDataType", &Clipboard::getDataType},
        {"readText", &Clipboard::readText},
        {"writeText", &Clipboard::writeText},
        {"readHtml", &Clipboard::readHtml},
        {"writeHtml", &Clipboard::writeHtml},
        {"readRtf", &Clipboard::readRtf},
        {"writeRtf", &Clipboard::writeRtf},
        {"readImg", &Clipboard::readImg},
        {"writeImg", &Clipboard::writeImg},
        {"addFile", &Clipboard::addFile},
        {"getFile", &Clipboard::getFile},
        {"clear", &Clipboard::clear},
    };

    UINT CF_HTML,CF_RTF;
}

Clipboard::Clipboard()
{
}

Clipboard::~Clipboard()
{
}

Clipboard* Clipboard::get()
{
    if(!clipboard) {
        CF_HTML = RegisterClipboardFormat(L"HTML Format");
        CF_RTF = RegisterClipboardFormat(L"Rich Text Format");
        clipboard = std::make_unique<Clipboard>();
	}
    return clipboard.get();
}

bool Clipboard::execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Clipboard::get()->*it->second)(param, result);
    return true;
}

void Clipboard::getDataType(const rapidjson::Value& params, JsonResult* result)
{
    if (!OpenClipboard(NULL)) {
        result->addErr("open clipboard err");
        return;
    }
    if (IsClipboardFormatAvailable(CF_HDROP)) {        
        result->addString("data", "file");
    }
    else if (IsClipboardFormatAvailable(CF_HTML)) {
        result->addString("data", "html");
    }
    else if (IsClipboardFormatAvailable(CF_RTF)) {
        result->addString("data", "rtf");
    }
    else if (IsClipboardFormatAvailable(CF_TEXT) || IsClipboardFormatAvailable(CF_UNICODETEXT)) {
        result->addString("data", "text");
    }
    else if (IsClipboardFormatAvailable(CF_BITMAP) || IsClipboardFormatAvailable(CF_DIB)) {
        result->addString("data", "img");
    }
    else {
        result->addString("data", "unknown");
    }
    CloseClipboard();
}

void Clipboard::readText(const rapidjson::Value& params, JsonResult* result)
{
    if (!OpenClipboard(NULL)) {
        result->addErr("open clipboard err");
        return;
    }
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (hData == NULL) {
        CloseClipboard();
        result->addErr("clipboard doesn't have text data");
        return;
    }
    wchar_t* pszText = (wchar_t*)GlobalLock(hData);
    if (pszText == NULL) {
        CloseClipboard();
        result->addErr("can not get clipboard text");
        return;
    }
    auto str = Util::convertToStr(pszText);
    GlobalUnlock(hData);
    CloseClipboard();
    result->addString("data", str);
}

void Clipboard::writeText(const rapidjson::Value& params, JsonResult* result)
{
    if (!OpenClipboard(NULL)) {
        result->addErr("open clipboard err");
        return;
    }
    EmptyClipboard();
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto text = Util::convertToWStr(arr[0].GetString());
    size_t length = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, length);
    if (hGlobal == NULL) {
        CloseClipboard();
        result->addErr("alloc global memory err");
        return;
    }
    auto pGlobal = (wchar_t*)GlobalLock(hGlobal);
    if (pGlobal == NULL) {
        CloseClipboard();
        result->addErr("global memory lock err");
        return;
    }
    memcpy(pGlobal, text.data(), length);
    GlobalUnlock(hGlobal);
    SetClipboardData(CF_UNICODETEXT, hGlobal);
    CloseClipboard();
}

void Clipboard::readImg(const rapidjson::Value& params, JsonResult* result)
{
    // 打开剪切板
    if (!OpenClipboard(nullptr)) {
        result->addErr("open clipboard err");
        return;
    }

    // 检查剪切板是否有位图
    if (!IsClipboardFormatAvailable(CF_BITMAP)) {
        result->addErr("no img");
        CloseClipboard();
        return;
    }

    // 获取位图句柄
    HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
    if (hBitmap == nullptr) {
        result->addErr("can not get img");
        CloseClipboard();
        return;
    }

    // 获取位图信息
    BITMAP bmp;
    if (GetObject(hBitmap, sizeof(BITMAP), &bmp) == 0) {
        result->addErr("can not get img info");
        CloseClipboard();
        return;
    }

    // 初始化 COM（确保在程序启动时调用 CoInitialize）
    HRESULT hr;
    wil::com_ptr<IWICImagingFactory> factory;
    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
    if (FAILED(hr)) {
        result->addErr("can not init WIC");
        CloseClipboard();
        return;
    }

    // 从 HBITMAP 创建 WIC 位图
    wil::com_ptr<IWICBitmap> wicBitmap;
    hr = factory->CreateBitmapFromHBITMAP(hBitmap, nullptr, WICBitmapUsePremultipliedAlpha, &wicBitmap);
    if (FAILED(hr)) {
        result->addErr("can not init WIC img");
        CloseClipboard();
        return;
    }

    // 创建内存流
    wil::com_ptr<IWICStream> wicStream;
    wil::com_ptr<IStream> memStream;
    hr = CreateStreamOnHGlobal(nullptr, TRUE, &memStream);
    if (FAILED(hr)) {
        result->addErr("can not create WIC stream1");
        CloseClipboard();
        return;
    }
    hr = factory->CreateStream(&wicStream);
    if (FAILED(hr)) {
        result->addErr("can not create WIC stream2");
        CloseClipboard();
        return;
    }
    hr = wicStream->InitializeFromIStream(memStream.get());
    if (FAILED(hr)) {
        result->addErr("can not create WIC stream3");
        CloseClipboard();
        return;
    }

    // 创建 PNG 编码器
    wil::com_ptr<IWICBitmapEncoder> encoder;
    hr = factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder);
    if (FAILED(hr)) {
        result->addErr("can not create png encoder1");
        CloseClipboard();
        return;
    }
    hr = encoder->Initialize(wicStream.get(), WICBitmapEncoderNoCache);
    if (FAILED(hr)) {
        result->addErr("can not create png encoder2");
        CloseClipboard();
        return;
    }

    // 创建帧并写入数据
    wil::com_ptr<IWICBitmapFrameEncode> frame;
    hr = encoder->CreateNewFrame(&frame, nullptr);
    if (FAILED(hr)) {
        result->addErr("can not create img frame");
        CloseClipboard();
        return;
    }
    hr = frame->Initialize(nullptr);
    if (FAILED(hr)) {
        result->addErr("can not init img frame");
        CloseClipboard();
        return;
    }
    hr = frame->SetSize(bmp.bmWidth, bmp.bmHeight);
    if (FAILED(hr)) {
        result->addErr("can not init img frame size");
        CloseClipboard();
        return;
    }
    WICPixelFormatGUID format = GUID_WICPixelFormat32bppRGBA;
    hr = frame->SetPixelFormat(&format);
    if (FAILED(hr)) {
        result->addErr("can not init pix format");
        CloseClipboard();
        return;
    }
    hr = frame->WriteSource(wicBitmap.get(), nullptr);
    if (FAILED(hr)) {
        result->addErr("can not write img frame");
        CloseClipboard();
        return;
    }
    hr = frame->Commit();
    if (FAILED(hr)) {
        result->addErr("can not commit img frame");
        CloseClipboard();
        return;
    }
    hr = encoder->Commit();
    if (FAILED(hr)) {
        result->addErr("can not commit img encoder");
        CloseClipboard();
        return;
    }

    // 获取 PNG 数据
    STATSTG stat;
    hr = memStream->Stat(&stat, STATFLAG_NONAME);
    if (FAILED(hr)) {
        result->addErr("can not stat img stream");
        CloseClipboard();
        return;
    }
    std::vector<unsigned char> pngBuffer(stat.cbSize.LowPart);
    HGLOBAL hGlobal;
    hr = GetHGlobalFromStream(memStream.get(), &hGlobal);
    if (FAILED(hr)) {
        result->addErr("can not get global stream");
        CloseClipboard();
        return;
    }
    void* data = GlobalLock(hGlobal);
    memcpy(pngBuffer.data(), data, stat.cbSize.LowPart);
    GlobalUnlock(hGlobal);

    // 创建共享缓冲区
    auto env12 = App::get()->env.try_query<ICoreWebView2Environment12>();
    wil::com_ptr<ICoreWebView2SharedBuffer> sharedBuffer;
    hr = env12->CreateSharedBuffer(stat.cbSize.LowPart, &sharedBuffer);
    if (FAILED(hr)) {
        result->addErr("can not create shared buffer");
        CloseClipboard();
        return;
    }
    wil::com_ptr<IStream> stream;
    hr = sharedBuffer->OpenStream(&stream);
    if (FAILED(hr)) {
        result->addErr("can not open stream");
        CloseClipboard();
        return;
    }
    hr = stream->Write(pngBuffer.data(), stat.cbSize.LowPart, nullptr);
    if (FAILED(hr)) {
        result->addErr("can not write png buffer");
        CloseClipboard();
        return;
    }
    result->addNumber("w", (int)bmp.bmWidth);
    result->addNumber("h", (int)bmp.bmHeight);
    result->returnBackSharedBuffer(sharedBuffer.get());
    sharedBuffer->Close();
    CloseClipboard();
    result->cancel = true;
}
void Clipboard::readImg1(const rapidjson::Value& params, JsonResult* result)
{
    if (!OpenClipboard(nullptr)) {
        result->addErr("can not open clipboard");
        return;
    }
    if (!IsClipboardFormatAvailable(CF_BITMAP)) {
        result->addErr("no img");
        CloseClipboard();
        return;
    }
    HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
    if (hBitmap == nullptr) {
        result->addErr("can not get img");
        CloseClipboard();
        return;
    }
    BITMAP bmp;
    if (GetObject(hBitmap, sizeof(BITMAP), &bmp) == 0) {
        result->addErr("can not get bitmap");
        CloseClipboard();
        return;
    }
    HDC hdc = CreateCompatibleDC(nullptr);
    if (hdc == nullptr) {
        result->addErr("create compatible DC err");
        CloseClipboard();
        return;
    }
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdc, hBitmap);
    if (hOldBitmap == nullptr) {
        result->addErr("select bitmap to hdc err");
        DeleteDC(hdc);
        CloseClipboard();
        return;
    }
    long long pixCount = bmp.bmWidth * 4 * bmp.bmHeight;
    BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), bmp.bmWidth, 0 - bmp.bmHeight, 1, 32, BI_RGB, pixCount, 0, 0, 0, 0 };
    std::vector<unsigned char> buffer(pixCount);
    if (GetDIBits(hdc, hBitmap, 0, bmp.bmHeight, buffer.data(), &bmi, DIB_RGB_COLORS) == 0) {
        result->addErr("get pix err");
        SelectObject(hdc, hOldBitmap);
        DeleteDC(hdc);
        CloseClipboard();
        return;
    }
    auto env12 = App::get()->env.try_query<ICoreWebView2Environment12>();
    wil::com_ptr<ICoreWebView2SharedBuffer> sharedBuffer;
    auto hr = env12->CreateSharedBuffer(pixCount, &sharedBuffer);
    wil::com_ptr<IStream> stream;
    sharedBuffer->OpenStream(&stream);
    stream->Write(buffer.data(), pixCount, nullptr);
    result->addNumber("w", (int)bmp.bmWidth);
    result->addNumber("h", (int)bmp.bmHeight);
    result->returnBackSharedBuffer(sharedBuffer.get());
    sharedBuffer->Close();
    result->cancel = true;
    // 清理资源
    SelectObject(hdc, hOldBitmap);
    DeleteDC(hdc);
    CloseClipboard();
}
void Clipboard::writeImg(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring imagePath = Util::convertToWStr(arr[0].GetString());
    if (!std::filesystem::exists(imagePath)) {
        result->addErr("Image file does not exist");
        return;
    }
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartupInput startupInput;
    if (Gdiplus::GdiplusStartup(&gdiplusToken, &startupInput, nullptr) != Gdiplus::Ok) {
        result->addErr("gdiplus init err");
        return;
    }
    Gdiplus::Bitmap bitmap(imagePath.c_str());
    if (bitmap.GetLastStatus() != Gdiplus::Ok) {
        Gdiplus::GdiplusShutdown(gdiplusToken);
        result->addErr("gdi bitmap init err");
        return;
    }
    auto width{ bitmap.GetWidth() }, height{ bitmap.GetHeight() };

    Gdiplus::BitmapData bitmapData;
    Gdiplus::Rect rect(0, 0, width, height);
    bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);
    bitmap.UnlockBits(&bitmapData);

    HDC screenDC = GetDC(nullptr);
    HDC memoryDC = CreateCompatibleDC(screenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(screenDC, width, height);
    DeleteObject(SelectObject(memoryDC, hBitmap));
    BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), width, 0 - height, 1, 32, BI_RGB, width * 4 * height, 0, 0, 0, 0 };
    SetDIBitsToDevice(memoryDC, 0, 0, width, height, 0, 0, 0, height, bitmapData.Scan0, &bmi, DIB_RGB_COLORS);

    if (!OpenClipboard(nullptr)) {
        DeleteObject(hBitmap);
        Gdiplus::GdiplusShutdown(gdiplusToken);
        result->addErr("open clipboard err");
        return;
    }
    if (!EmptyClipboard() || !SetClipboardData(CF_BITMAP, hBitmap)) {
        CloseClipboard();
        DeleteObject(hBitmap);
        Gdiplus::GdiplusShutdown(gdiplusToken);
        result->addErr("set clipboard data err");
        return;
    }
    CloseClipboard();
    ReleaseDC(nullptr, screenDC);
    DeleteDC(memoryDC);
    DeleteObject(hBitmap);
    Gdiplus::GdiplusShutdown(gdiplusToken);
}

void Clipboard::getFile(const rapidjson::Value& params, JsonResult* result)
{
    if (!OpenClipboard(nullptr)) {
        result->addErr("open clipboard err");
        return;
    }
    HANDLE hClipboardData = GetClipboardData(CF_HDROP);
    if (!hClipboardData) {
        CloseClipboard();
        result->addErr("No files found in clipboard");
        return;
    }
    DROPFILES* pDropFiles = static_cast<DROPFILES*>(GlobalLock(hClipboardData));
    if (!pDropFiles) {
        CloseClipboard();
        result->addErr("Failed to lock clipboard data");
        return;
    }
    rapidjson::Value array(rapidjson::kArrayType);
    if (pDropFiles->fWide) {
        wchar_t* fileName = reinterpret_cast<wchar_t*>(reinterpret_cast<char*>(pDropFiles) + pDropFiles->pFiles);
        while (*fileName) {
            auto str = Util::convertToStr(fileName);
            rapidjson::Value val;
            val.SetString(str.data(), str.length(), result->getAllocator());
            array.PushBack(val, result->getAllocator());
            fileName += wcslen(fileName) + 1;
        }
    }
    GlobalUnlock(hClipboardData);
    CloseClipboard();
    result->addValue("data", std::move(array));
}

void Clipboard::addFile(const rapidjson::Value& params, JsonResult* result)
{
    if (!OpenClipboard(NULL)) {
        result->addErr("open clipboard err");
        return;
    }
    EmptyClipboard();
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::vector<std::wstring> filePaths;
    for (auto& val : arr)
    {
        auto text = Util::convertToWStr(val.GetString());
        filePaths.push_back(std::move(text));
    }
    size_t totalSize = sizeof(DROPFILES);
    for (const auto& path : filePaths) {
        totalSize += (path.length() + 1) * sizeof(wchar_t);
    }
    totalSize += sizeof(wchar_t);
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, totalSize);
    if (!hGlobal) {
        CloseClipboard();
        result->addErr("alloc global memory err");
        return;
    }
    DROPFILES* pDropFiles = static_cast<DROPFILES*>(GlobalLock(hGlobal));
    if (!pDropFiles) {
        GlobalFree(hGlobal);
        CloseClipboard();
        result->addErr("global lock err");
        return;
    }
    pDropFiles->pFiles = sizeof(DROPFILES);
    pDropFiles->fWide = TRUE;
    wchar_t* dest = reinterpret_cast<wchar_t*>(pDropFiles + 1);
    for (const auto& path : filePaths) {
        wcscpy_s(dest, path.length() + 1, path.c_str());
        dest += path.length() + 1;
    }
    *dest = L'\0';
    GlobalUnlock(hGlobal);
    if (!SetClipboardData(CF_HDROP, hGlobal)) {
        GlobalFree(hGlobal);
        CloseClipboard();
        result->addErr("set data err");
    }
    CloseClipboard();
}

void Clipboard::readHtml(const rapidjson::Value& params, JsonResult* result)
{
    if (!OpenClipboard(NULL)) {
        result->addErr("Open clipboard error: " + std::to_string(GetLastError()));
        return;
    }
    if (!IsClipboardFormatAvailable(CF_HTML)) {
        result->addErr("No HTML data in clipboard");
        CloseClipboard();
        return;
    }
    HGLOBAL hData = GetClipboardData(CF_HTML);
    if (hData == NULL) {
        result->addErr("Cannot get HTML data: " + std::to_string(GetLastError()));
        CloseClipboard();
        return;
    }
    // 锁定内存
    const char* htmlData = static_cast<const char*>(GlobalLock(hData));
    if (htmlData == NULL) {
        result->addErr("Cannot lock memory: " + std::to_string(GetLastError()));
        CloseClipboard();
        return;
    }
    // 构造字符串
    std::string data(htmlData);
    // 解锁内存并关闭剪切板
    GlobalUnlock(hData);
    CloseClipboard();
    // 查找 StartFragment 和 EndFragment
    size_t startFragmentPos = data.find("<!--StartFragment-->");
    size_t endFragmentPos = data.find("<!--EndFragment-->");
    if (startFragmentPos == std::string::npos || endFragmentPos == std::string::npos) {
        result->addErr("Failed to find StartFragment or EndFragment markers");
        return;
    }
    // 计算起始和结束位置
    size_t contentStart = startFragmentPos + 20; // 跳过 "<!--StartFragment-->" 的长度
    if (contentStart >= endFragmentPos) {
        result->addErr("Invalid fragment positions: start >= end");
        return;
    }
    // 提取 <!--StartFragment--> 和 <!--EndFragment--> 之间的内容
    std::string fragment = data.substr(contentStart, endFragmentPos - contentStart);
    result->addString("data", fragment);
}

void Clipboard::writeHtml(const rapidjson::Value& params, JsonResult* result)
{
    //todo src Url
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::string fragment = arr[0].GetString();
    constexpr std::string_view header =
        "Version:0.9\r\n"
        "StartHTML:{:08d}\r\n"
        "EndHTML:{:08d}\r\n"
        "StartFragment:{:08d}\r\n"
        "EndFragment:{:08d}\r\n"
        "StartSelection:{:08d}\r\n"
        "EndSelection:{:08d}\r\n"
        "SourceURL:about:blank\r\n";
    std::string_view htmlPrefix = "<html>\r\n<head><meta charset=\"UTF-8\"></head>\r\n<body>\r\n<!--StartFragment-->";
    std::string_view htmlSuffix = "<!--EndFragment-->\r\n</body>\r\n</html>";

    std::string cfHtmlHeader = std::format(header, 0, 0, 0, 0, 0, 0);
    size_t headerLen = cfHtmlHeader.length();
    size_t startHtml = headerLen;
    size_t startFragment = startHtml + htmlPrefix.length();
    size_t endFragment = startFragment + fragment.length();
    size_t endHtml = endFragment + htmlSuffix.length();
    cfHtmlHeader = std::format(header, startHtml, endHtml, startFragment, endFragment, startFragment, endFragment);
    std::string sHtml;
    sHtml.reserve(endHtml + 1);
    sHtml.append(cfHtmlHeader).append(htmlPrefix).append(fragment).append(htmlSuffix);
    if (!OpenClipboard(NULL)) {
        result->addErr("open clipboard err");
        return;
    }
    EmptyClipboard();
    HGLOBAL hGlobal = GlobalAlloc(GMEM_DDESHARE, sHtml.size() + 1);
    if (hGlobal == NULL) {
        CloseClipboard();
        result->addErr("alloc global memory err");
        return;
    }
    char* pchData = static_cast<char*>(GlobalLock(hGlobal));
    if (pchData == NULL) {
        GlobalFree(hGlobal);
        CloseClipboard();
        result->addErr("lock global memory err");
        return;
    }
    strcpy_s(pchData, sHtml.size() + 1, sHtml.c_str());
    GlobalUnlock(hGlobal);
    SetClipboardData(CF_HTML, hGlobal);
    CloseClipboard();
}

void Clipboard::readRtf(const rapidjson::Value& params, JsonResult* result)
{
    //todo 如果剪切板里存储的是普通文本，那么有可能读出来的也是普通文本，此时应该给客户端报个异常才对
    if (!OpenClipboard(NULL)) {
        result->addErr("Open clipboard error: " + std::to_string(GetLastError()));
        return;
    }
    if (!IsClipboardFormatAvailable(CF_RTF)) {
        result->addErr("No HTML data in clipboard");
        CloseClipboard();
        return;
    }
    HGLOBAL hData = GetClipboardData(CF_RTF);
    if (hData == NULL) {
        result->addErr("Cannot get HTML data: " + std::to_string(GetLastError()));
        CloseClipboard();
        return;
    }
    const char* data = static_cast<const char*>(GlobalLock(hData));
    if (data == NULL) {
        result->addErr("Cannot lock memory: " + std::to_string(GetLastError()));
        CloseClipboard();
        return;
    }
    // 解锁内存并关闭剪切板
    GlobalUnlock(hData);
    CloseClipboard();
    result->addString("data", data);
}

void Clipboard::writeRtf(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring text1 = Util::convertToWStr(arr[0].GetString());
    std::string text = Util::convertToAnsi(text1);
    if (!OpenClipboard(NULL)) {
        result->addErr("open clipboard err");
        return;
    }
    EmptyClipboard();
    size_t length = (text.size() + 1);
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, length);
    if (hGlobal == NULL) {
        CloseClipboard();
        result->addErr("alloc global memory err");
        return;
    }
    auto pGlobal = (char*)GlobalLock(hGlobal);
    if (pGlobal == NULL) {
        CloseClipboard();
        result->addErr("global memory lock err");
        return;
    }
    strcpy_s(pGlobal, length, text.c_str());
    GlobalUnlock(hGlobal);
    SetClipboardData(CF_RTF, hGlobal);
    CloseClipboard();
}

void Clipboard::clear(const rapidjson::Value& params, JsonResult* result)
{
    if (!OpenClipboard(NULL)) {
        result->addErr("open clipboard err");
        return;
    }
    EmptyClipboard();
    CloseClipboard();
}


