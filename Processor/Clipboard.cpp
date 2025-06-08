#include <pch.h>
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

void Clipboard::readImage(const rapidjson::Value& params, JsonResult* result)
{
}

void Clipboard::writeImage(const rapidjson::Value& params, JsonResult* result)
{
}

void Clipboard::readFile(const rapidjson::Value& params, JsonResult* result)
{
}

void Clipboard::writeFile(const rapidjson::Value& params, JsonResult* result)
{
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
}

void Clipboard::readBookmark(const rapidjson::Value& params, JsonResult* result)
{
}

void Clipboard::writeBookmark(const rapidjson::Value& params, JsonResult* result)
{
}

void Clipboard::clear(const rapidjson::Value& params, JsonResult* result)
{
}


