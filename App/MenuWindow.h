#pragma once
class MenuWindow
{
public:
	MenuWindow();
	~MenuWindow();
	static MenuWindow* get();
	void show(const POINT& pt,std::map<int,std::wstring>& menus);
public:	
	HWND hwnd;
	int x, y, w, h;
	int srcId;
	std::vector<int> ids;
	std::vector<std::wstring> texts;
	int hoverIndex{ -1 };
private:
	void createWindow();
	static LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:

};

