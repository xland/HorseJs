#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Screen
{
public:
	Screen();
	~Screen();
	static Screen* get();
	static bool excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	void getAll(const rapidjson::Value& params, JsonResult* result);
private:
	static BOOL CALLBACK enumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
};

