#include <pch.h>
#include "Clipboard.h"

namespace {
    std::unique_ptr<Clipboard> clipboard;
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
        clipboard = std::make_unique<Clipboard>();
	}
    return clipboard.get();
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
    result->addString("result", str);
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


