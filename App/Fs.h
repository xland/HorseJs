#pragma once
#include "Util.h"
#include "JsonParsor.h"
class BrowserWindow;
class Fs
{
public:
	Fs();
	~Fs();
	static Fs* get();

	void getFileInfo(BrowserWindow* win,const rapidjson::Value& params, JsonParsor& result);
	void exists(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result);
	void readFile(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result);
	void readFileChunk(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result);
	void writeFile(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result);
	void writeFileChunk(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result);

	void removeFile(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result);
	void removeDir(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result);
	void createDir(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result);
	void listDir(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result);
	void copyFile(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result);
	void moveFile(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result);
	void renameFile(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result);
	void watch(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result);
private:
};

