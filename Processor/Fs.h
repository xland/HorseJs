#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class BrowserWindow;
class Fs
{
public:
	Fs();
	~Fs();
	static Fs* get();
	static bool execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	void getFileInfo(const rapidjson::Value& params, JsonResult* result);
	void exists(const rapidjson::Value& params, JsonResult* result);
	void readFile(const rapidjson::Value& params, JsonResult* result);
	void readFileChunk(const rapidjson::Value& params, JsonResult* result);
	void writeFile(const rapidjson::Value& params, JsonResult* result);
	void writeFileChunk(const rapidjson::Value& params, JsonResult* result);
	void delPath(const rapidjson::Value& params, JsonResult* result);
	void removePath(const rapidjson::Value& params, JsonResult* result);
	void createFile(const rapidjson::Value& params, JsonResult* result);
	void createDir(const rapidjson::Value& params, JsonResult* result);
	void ensurePath(const rapidjson::Value& params, JsonResult* result);
	void listDir(const rapidjson::Value& params, JsonResult* result);
	void copyPath(const rapidjson::Value& params, JsonResult* result);
	void movePath(const rapidjson::Value& params, JsonResult* result);
	void renamePath(const rapidjson::Value& params, JsonResult* result);
	void getPath(const rapidjson::Value& params, JsonResult* result);
	void watch(const rapidjson::Value& params, JsonResult* result);
	void stopWatch(const rapidjson::Value& params, JsonResult* result);
	void packHorse(const rapidjson::Value& params, JsonResult* result);
	void createShortcut(const rapidjson::Value& params, JsonResult* result);
	void openFile(const rapidjson::Value& params, JsonResult* result);
private:
	bool delDirRecursive(const std::wstring& dirPath);
	void getKnownPath(const GUID& type, JsonResult* result);
	void enumFiles(const std::wstring& baseDir, const std::wstring& currentDir, std::vector<std::wstring>& fileList);
	bool addResToExe(const HANDLE& handle, std::wstring& resName, const std::wstring& resDir);
};

