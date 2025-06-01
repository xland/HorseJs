#pragma once
#include "Util.h"
#include "JsonResult.h"

class BrowserWindow;
class Dialog
{
public:
	Dialog();
	~Dialog();
	static Dialog* get();
	void openPathDialog(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
private:
};

