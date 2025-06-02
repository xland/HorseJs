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
}

void Clipboard::writeText(const rapidjson::Value& params, JsonResult* result)
{
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


