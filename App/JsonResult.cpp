#include "JsonResult.h"

JsonResult::JsonResult()
{
}

JsonResult::~JsonResult()
{
}

void JsonResult::addErr(const std::string& value)
{
    ok = false;
    addBool("ok", false);
    addString("err", value);
}