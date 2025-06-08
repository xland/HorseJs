#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Lib
{
public:
	Lib();
	~Lib();
	static Lib* get();
	static bool execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	void load(const rapidjson::Value& params, JsonResult* result);
	void free(const rapidjson::Value& params, JsonResult* result);
	void call(const rapidjson::Value& params, JsonResult* result);
private:
};

