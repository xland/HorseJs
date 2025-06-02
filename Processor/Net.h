#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Net
{
public:
	Net();
	~Net();
	static Net* get();
	void getAddress(const rapidjson::Value& params, JsonResult* result);
private:
};

