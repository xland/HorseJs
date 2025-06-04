#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Lib
{
public:
	Lib();
	~Lib();
	static Lib* get();
	void load(const rapidjson::Value& params, JsonResult* result);
private:
};

