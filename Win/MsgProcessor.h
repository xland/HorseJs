#pragma once
#include "../App/Util.h"

class BrowserWindow;
class Page;
class JsonParsor;
class MsgProcessor
{
public:
	MsgProcessor(BrowserWindow* win, Page* page);
	~MsgProcessor();
	void processStr(const std::string& msgStr);
private:
	BrowserWindow* win;
	Page* page;
private:
	void processPage(const int& methodId, const rapidjson::Value& params, JsonParsor& parsor);
	void processWin(const int& methodId, const rapidjson::Value& params, JsonParsor& parsor);
};

