#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Menu
{
public:
	Menu();
	~Menu();
	static Menu* get();
	static bool excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	void create(const rapidjson::Value& params, JsonResult* result);
private:
};

