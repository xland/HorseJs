#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Dll
{
public:
	Dll();
	~Dll();
	static Dll* get();
	static bool execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	void load(const rapidjson::Value& params, JsonResult* result);
	void free(const rapidjson::Value& params, JsonResult* result);
	void invoke(const rapidjson::Value& params, JsonResult* result);
	void on(const rapidjson::Value& params, JsonResult* result);
	void off(const rapidjson::Value& params, JsonResult* result);
private:
	static void eventCB(const char* eventName, const char* eventData);
};

