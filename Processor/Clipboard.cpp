#include <pch.h>
#include "Clipboard.h"

namespace {
    std::unique_ptr<Clipboard> clipboard;
    UINT CF_HTML;
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
        clipboard = std::make_unique<Clipboard>();
	}
    return clipboard.get();
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
    result->returnBack();
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
    result->returnBack();
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
    result->returnBack();
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
    // 打开剪切板
    if (!OpenClipboard(NULL)) {
        result->addErr("Open clipboard error: " + std::to_string(GetLastError()));
        return;
    }

    // 检查 HTML 格式
    if (!IsClipboardFormatAvailable(CF_HTML)) {
        result->addErr("No HTML data in clipboard");
        CloseClipboard();
        return;
    }

    // 获取 HTML 数据句柄
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
    result->returnBack();
}

void Clipboard::writeHtml(const rapidjson::Value& params, JsonResult* result)
{
}

void Clipboard::readRtf(const rapidjson::Value& params, JsonResult* result)
{
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


