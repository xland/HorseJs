#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Net
{
public:
	Net();
	~Net();
	static Net* get();
	static bool excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	void getAddress(const rapidjson::Value& params, JsonResult* result);
private:
};

