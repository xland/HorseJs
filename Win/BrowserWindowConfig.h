#pragma once
#include <limits>
#include "../App/Util.h"
class BrowserWindowConfig
{
public:
	BrowserWindowConfig(const rapidjson::Value& config);
	~BrowserWindowConfig();
public:
	int x{ 100 }, y{ 100 }, w{ 1000 }, h{ 800 };
	int minWidth{ 0 }, minHeight{ 0 }, maxWidth{ std::numeric_limits<int>::max() }, maxHeight{ std::numeric_limits<int>::max() };
	bool maximize{ false }, visible{ true }, frame{ true }, shadow{ true }, maximizable{ true }, resizable{true};
	std::wstring title{ L"HorseJs" };
private:
	void setSize(const rapidjson::Value& config);
};

