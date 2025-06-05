#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Tray
{
public:
	Tray();
	~Tray();
	static Tray* get();
	static bool excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
private:
};

