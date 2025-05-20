#pragma once
#include "../App/Util.h"
class BrowserWindowConfig
{
public:
	BrowserWindowConfig(const rapidjson::Value& config);
	~BrowserWindowConfig();
public:
	int x{ 100 }, y{ 100 }, w{ 1000 }, h{ 800 };
	bool maximize{ false }, visible{ true }, frame{ true }, shadow{ true }, maximizable{ true };
	std::wstring title{ L"HorseJs" };
private:
};

