#pragma once
#include <cstdarg>
#include "Util.h"

class BrowserWindow;
class Page;
class JsonParsor;
class MsgProcessor
{
public:
	MsgProcessor();
	~MsgProcessor();
	static void init();
	static MsgProcessor* get();
	void processStr(const std::string& msgStr);
	//void emit(const int& classId, const int& eventId, int count, ...);
public:
private:
	//void processPage(const std::string& methodName, const rapidjson::Value& params, JsonParsor& parsor);
	//void processWin(const std::string& methodName, const rapidjson::Value& params, JsonParsor& parsor);
	//void processFs(const std::string& methodName, const rapidjson::Value& params, JsonParsor& parsor);
	//void processHorse(const std::string& methodName, const rapidjson::Value& params, JsonParsor& parsor);
	//void emitWin(const int& eventId, int count, va_list args, JsonParsor& parsor);
	//void emitPage(const int& eventId, int count, va_list args, JsonParsor& parsor);
};

