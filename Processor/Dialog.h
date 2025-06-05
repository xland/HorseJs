#pragma once
#include "../App/Util.h"
#include "JsonResult.h"

class BrowserWindow;
class Dialog
{
public:
	Dialog();
	~Dialog();
	static Dialog* get();
	static bool excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	void openPathDialog(const rapidjson::Value& params, JsonResult* result);
private:

};

