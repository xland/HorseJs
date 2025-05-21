#pragma once
#include <cstdarg>
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
	void emit(const int& classId, const int& eventId, int count, ...);
private:
	BrowserWindow* win;
	Page* page;
private:
	void processPage(const int& methodId, const rapidjson::Value& params, JsonParsor& parsor);
	void processWin(const int& methodId, const rapidjson::Value& params, JsonParsor& parsor);
	void emitWin(const int& eventId, int count, va_list args);
	void emitPage(const int& eventId, int count, va_list args);
};

