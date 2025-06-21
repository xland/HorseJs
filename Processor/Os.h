#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Os
{
public:
	Os();
	~Os();
	static Os* get();
	static bool execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	void getVersion(const rapidjson::Value& params, JsonResult* result);
	void createShortcut(const rapidjson::Value& params, JsonResult* result);
	void getCPUID(const rapidjson::Value& params, JsonResult* result);
	void getDiskSerialNumber(const rapidjson::Value& params, JsonResult* result);
	void getUserLang(const rapidjson::Value& params, JsonResult* result);
	void getOsLang(const rapidjson::Value& params, JsonResult* result);
	void getOsColor(const rapidjson::Value& params, JsonResult* result);
	void showItemInFolder(const rapidjson::Value& params, JsonResult* result);
	void openFile(const rapidjson::Value& params, JsonResult* result);
	void preventSleep(const rapidjson::Value& params, JsonResult* result);
	void stopPreventSleep(const rapidjson::Value& params, JsonResult* result);
	void getIpAddr(const rapidjson::Value& params, JsonResult* result);
	void showNotify(const rapidjson::Value& params, JsonResult* result);
	void createTray(const rapidjson::Value& params, JsonResult* result);
	void destroyTray(const rapidjson::Value& params, JsonResult* result);
	void credWrite(const rapidjson::Value& params, JsonResult* result);
	void credRead(const rapidjson::Value& params, JsonResult* result);
	void credDel(const rapidjson::Value& params, JsonResult* result);
	void spawn(const rapidjson::Value& params, JsonResult* result);
	void on(const rapidjson::Value& params, JsonResult* result);
	void off(const rapidjson::Value& params, JsonResult* result);
private:
};

