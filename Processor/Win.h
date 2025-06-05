#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class BrowserWindow;
class Win
{
public:
	Win();
	~Win();
	static Win* get();
	static bool excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	void show(const rapidjson::Value& params, JsonResult* result);
	void hide(const rapidjson::Value& params, JsonResult* result);
	void maximize(const rapidjson::Value& params, JsonResult* result);
	void minimize(const rapidjson::Value& params, JsonResult* result);
	void restore(const rapidjson::Value& params, JsonResult* result);
	void flash(const rapidjson::Value& params, JsonResult* result);
	void close(const rapidjson::Value& params, JsonResult* result);
	void destroy(const rapidjson::Value& params, JsonResult* result);
	void startDrag(const rapidjson::Value& params, JsonResult* result);
	void openWindow(const rapidjson::Value& params, JsonResult* result);
	void setResizable(const rapidjson::Value& params, JsonResult* result);
	void resize(const rapidjson::Value& params, JsonResult* result);
	void addEventListener(const rapidjson::Value& params, JsonResult* result);
	void removeEventListener(const rapidjson::Value& params, JsonResult* result);
};

