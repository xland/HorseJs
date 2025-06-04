#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Os
{
public:
	Os();
	~Os();
	static Os* get();
	void getVersion(const rapidjson::Value& params, JsonResult* result);
private:
};

