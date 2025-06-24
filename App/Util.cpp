#include <pch.h>
#include "Util.h"

std::string Util::readFile(const std::wstring& filePath)
{
    if (!std::filesystem::exists(filePath)) {
        MessageBox(NULL, L"file path error.", L"Error", MB_OK | MB_ICONERROR);
        PostQuitMessage(0);
        return "";
    }
    std::ifstream file(filePath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

std::wstring Util::convertToWStr(const char* str)
{
    if (!str) return std::wstring();
    int count = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
    if (count == 0) return std::wstring();
    std::vector<wchar_t> buffer(count);
    MultiByteToWideChar(CP_UTF8, 0, str, -1, buffer.data(), count);
    return std::wstring(buffer.data(), buffer.size() - 1);
}

std::string Util::convertToStr(const std::wstring& wstr)
{
    const int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    return str;
}

std::wstring Util::convertAnsiToWstring(const char* str) {
    if (!str) return std::wstring();
    int count = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
    if (count <= 0) return std::wstring();
    std::wstring wstr(count - 1, 0);
    MultiByteToWideChar(CP_ACP, 0, str, -1, &wstr[0], count);
    return wstr;
}

std::string Util::convertWstringToUtf8(const std::wstring& wstr) {
    int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (count <= 0) return std::string();
    std::string utf8Str(count - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8Str[0], count, nullptr, nullptr);
    return utf8Str;
}

std::string Util::colorToHex(COLORREF color) {
    int r = GetRValue(color);
    int g = GetGValue(color);
    int b = GetBValue(color);
    std::stringstream ss;
    ss << "#"
        << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << r
        << std::setw(2) << std::setfill('0') << g
        << std::setw(2) << std::setfill('0') << b;
    return ss.str();
}

std::wstring Util::getContentType(const std::wstring& fileName)
{
    static const std::map<std::string, std::wstring> mimeTypes = {
            {".html", L"text/html"},
            {".htm",  L"text/html"},
            {".js",   L"application/javascript"},
            {".css",  L"text/css"},
            {".json", L"application/json"},
            {".png",  L"image/png"},
            {".jpg",  L"image/jpeg"},
            {".jpeg", L"image/jpeg"},
            {".gif",  L"image/gif"},
            {".svg",  L"image/svg+xml"},
            {".ico",  L"image/x-icon"},
            {".woff", L"font/woff"},
            {".woff2",L"font/woff2"},
            {".ttf",  L"font/ttf"},
            {".eot",  L"application/vnd.ms-fontobject"},
            {".txt",  L"text/plain"},
            {".mp3",  L"audio/mpeg"},
            {".mp4",  L"video/mp4"}
    };
    std::filesystem::path path(fileName);
    auto ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
    auto it = mimeTypes.find(ext);
    if (it != mimeTypes.end()) {
        return it->second;
    }
    return L"application/octet-stream";
}

void Util::printTime()
{
    SYSTEMTIME st;
    GetSystemTime(&st);
    auto str = std::format(L"\n{}-{}-{} {}:{}:{}:{}", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    OutputDebugString(str.data());
}

int Util::bitmapToPngData(HBITMAP bitmap, std::vector<std::byte>& pngData, int& w, int& h)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
    IStream* pngStream = nullptr;
    auto hr = CreateStreamOnHGlobal(nullptr, TRUE, &pngStream);
    if (FAILED(hr)) {
        return -1;
    }
    Gdiplus::Bitmap bmp(bitmap, nullptr);
    CLSID pngClsid = *Util::getPngClsid();
    if (pngClsid == CLSID_NULL) {
        return -1;
    }
    // 将 Bitmap 保存为 PNG 到内存流
    bmp.Save(pngStream, &pngClsid, nullptr);
    w = bmp.GetWidth();
    h = bmp.GetHeight();
    // 获取流大小并读取数据
    STATSTG stat;
    pngStream->Stat(&stat, STATFLAG_NONAME);
    ULONG pngSize = (ULONG)stat.cbSize.QuadPart;
    pngData.resize(pngSize);
    LARGE_INTEGER zero = {};
    pngStream->Seek(zero, STREAM_SEEK_SET, nullptr);
    ULONG bytesRead;
    pngStream->Read(pngData.data(), pngSize, &bytesRead);
    pngStream->Release();
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
}

CLSID* Util::getPngClsid()
{
    static CLSID pngClsid = [] {
        using namespace Gdiplus;
        UINT num = 0;          // 编码器数量
        UINT size = 0;         // 编码器数组大小（字节）
        GetImageEncodersSize(&num, &size);
        if (size == 0) {
            return CLSID_NULL;
        }
        std::vector<BYTE> buffer(size);
        ImageCodecInfo* pImageCodecInfo = reinterpret_cast<ImageCodecInfo*>(buffer.data());
        if (GetImageEncoders(num, size, pImageCodecInfo) != Ok) {
            return CLSID_NULL;
        }
        for (UINT i = 0; i < num; ++i)
        {
            //todo 目前只用到了png，用到其他的编码之后再扩展此函数
            if (wcscmp(pImageCodecInfo[i].MimeType, L"image/png") == 0) 
            {
                return pImageCodecInfo[i].Clsid;
            }
        }
        return CLSID_NULL;
    }();
    return &pngClsid;
}
