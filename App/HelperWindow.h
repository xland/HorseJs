#pragma once
class TrayData;
class HelperWindow
{
public:
	HelperWindow();
	~HelperWindow();
	static HelperWindow* get();
	void startCreateTray(TrayData* trayData);
	HWND hwnd;
private:
	void createWindow();
	void createTray(TrayData* trayData);
	static LRESULT CALLBACK winProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT winMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	std::vector<NOTIFYICONDATA*> trays;
	std::unordered_map<int, HMENU> menus;
	std::unordered_map<int, int> trayWinId;
};

