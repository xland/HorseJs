#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Process
{
public:
	Process();
	~Process();
	static Process* get();
	static bool excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
private:
};

