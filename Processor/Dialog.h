#pragma once
#include "../App/Util.h"
#include "JsonResult.h"

using filterType = std::vector<std::pair<std::wstring, std::wstring>>;
class BrowserWindow;
class Dialog
{
public:
	Dialog();
	~Dialog();
	static Dialog* get();
	static bool excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	void openPathDialog(const rapidjson::Value& params, JsonResult* result);
private:
	void showPathDialog(JsonResult* result,
		const std::wstring&& title,
		const std::wstring&& okBtnText,
		const std::wstring&& defaultDir,
		const FILEOPENDIALOGOPTIONS&& option,
		const filterType&& filter);
};

