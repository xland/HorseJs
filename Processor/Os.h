#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Os
{
public:
	Os();
	~Os();
	static Os* get();
	static bool excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	void getVersion(const rapidjson::Value& params, JsonResult* result);
private:
};

