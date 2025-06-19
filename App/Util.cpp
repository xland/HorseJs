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

void Util::printTime()
{
    SYSTEMTIME st;
    GetSystemTime(&st);
    auto str = std::format(L"\n{}-{}-{} {}:{}:{}:{}", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    OutputDebugString(str.data());
}
