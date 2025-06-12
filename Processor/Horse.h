#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Horse
{
public:
	Horse();
	~Horse();
	static Horse* get();
	static bool execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	void getConfig(const rapidjson::Value& params, JsonResult* result);
	void createWindow(const rapidjson::Value& params, JsonResult* result);
	void getExeVer(const rapidjson::Value& params, JsonResult* result);
	void getHorseVer(const rapidjson::Value& params, JsonResult* result);
	void quit(const rapidjson::Value& params, JsonResult* result);
	void exit(const rapidjson::Value& params, JsonResult* result);
	void relaunch(const rapidjson::Value& params, JsonResult* result);
private:

};

