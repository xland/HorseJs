#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Notification
{
public:
	Notification();
	~Notification();
	static Notification* get();
	void show(const rapidjson::Value& params, JsonResult* result);
private:
};

