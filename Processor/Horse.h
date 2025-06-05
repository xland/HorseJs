#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Horse
{
public:
	Horse();
	~Horse();
	static Horse* get();
	static bool excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	void getConfig(const rapidjson::Value& params, JsonResult* result);
	void createWindow(const rapidjson::Value& params, JsonResult* result);
private:
};

