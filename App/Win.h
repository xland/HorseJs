#pragma once
#include "Util.h"
#include "JsonResult.h"
class BrowserWindow;
class Win
{
public:
	Win();
	~Win();
	static Win* get();

	void show(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void hide(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void maximize(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void minimize(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void restore(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void flash(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void close(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void destroy(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void startDrag(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void openWindow(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void setResizable(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void resize(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void addEventListener(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void removeEventListener(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
};

