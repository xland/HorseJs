#pragma once
#include "Util.h"
#include "JsonResult.h"
class BrowserWindow;
class Fs
{
public:
	Fs();
	~Fs();
	static Fs* get();

	void getFileInfo(BrowserWindow* win,const rapidjson::Value& params, JsonResult& result);
	void exists(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void readFile(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void readFileChunk(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void writeFile(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void writeFileChunk(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);

	void delPath(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void removePath(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void createDir(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void listDir(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void copyFile(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void moveFile(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void renameFile(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
	void watch(BrowserWindow* win, const rapidjson::Value& params, JsonResult& result);
private:
	bool delDirRecursive(const std::wstring& dirPath);
};

