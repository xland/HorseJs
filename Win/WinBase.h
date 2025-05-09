#pragma once
#include "../App/Util.h"

class WinBase
{
public:
	WinBase(const int& x, const int& y, const int& w, const int& h, 
		const bool& visible, const std::wstring& title);
	~WinBase();
	void initWindow();
	static WinBase* create(const rapidjson::Value& config);
protected:
private:
	WNDCLASSEX* regWinClass();
	void show();
	static LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	HWND hwnd;
	int x, y, w, h;
	bool visible;
	std::wstring title;
};

