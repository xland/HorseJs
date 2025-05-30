#pragma once
#include "Util.h"
#include "JsonParsor.h"

class BrowserWindow;
class Dialog
{
public:
	Dialog();
	~Dialog();
	static Dialog* get();
	void openPathDialog(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result);
private:
};

