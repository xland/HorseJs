#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Screen
{
public:
	Screen();
	~Screen();
	static Screen* get();
	static bool excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
private:
};

