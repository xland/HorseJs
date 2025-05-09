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
    int count = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_UTF8, 0, str, -1, &wstr[0], count);
    if (wstr.length() == 1 && str[0] == L'\0') {
        wstr = std::wstring{};
    }
    return wstr;
}

std::string Util::convertToStr(const std::wstring& wstr)
{
    const int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    return str;
}
