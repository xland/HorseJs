#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class TrayData
{
public:
	int id;
	std::map<int, std::wstring> menus;
	std::wstring tip;
	int winId;
private:

};
class Tray
{
public:
	Tray();
	~Tray();
	static Tray* get();
	static bool excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	void create(const rapidjson::Value& params, JsonResult* result);
private:
};

