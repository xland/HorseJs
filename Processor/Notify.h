#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Notify
{
public:
	Notify();
	~Notify();
	static Notify* get();
	void show(const rapidjson::Value& params, JsonResult* result);
private:
};

