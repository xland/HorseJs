#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Process
{
public:
	Process();
	~Process();
	static Process* get();
	static bool execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	void exec(const rapidjson::Value& params, JsonResult* result);
private:
};

