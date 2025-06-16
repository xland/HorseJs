#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Notify
{
public:
	Notify();
	~Notify();
	static Notify* get();
	static bool execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	void show(const rapidjson::Value& params, JsonResult* result);
private:
	std::wstring getIcon();
	void createShortcutWithAUMID(const std::wstring& aumid);
};

