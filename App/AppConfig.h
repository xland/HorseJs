#pragma once
#include "Util.h"
class AppConfig
{
public:
	AppConfig();
	~AppConfig();
	rapidjson::Value& getFirstWindowConfig();
	void releaseJsonDoc();
public:
	bool quitWhenAllWindowClosed{ true };
	std::string appId;
private:
	rapidjson::Document jsonDoc;
};

