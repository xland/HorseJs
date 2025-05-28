#pragma once
#include "Util.h"
#include "JsonParsor.h"
class Fs
{
public:
	Fs();
	~Fs();
	static Fs* get();
	void addDirAsExeRes(const std::wstring& dirPath,const std::wstring& exePath);
	void addFileAsExeRes();
	void addExeRes();

	void stat(const rapidjson::Value& params, JsonParsor& result);
	void exists(const rapidjson::Value& params, JsonParsor& result);
	void readFile(const rapidjson::Value& params, JsonParsor& result);
	void writeFile(const rapidjson::Value& params, JsonParsor& result);
	void removeFile(const rapidjson::Value& params, JsonParsor& result);
	void removeDir(const rapidjson::Value& params, JsonParsor& result);
	void createDir(const rapidjson::Value& params, JsonParsor& result);
	void listDir(const rapidjson::Value& params, JsonParsor& result);
	void copyFile(const rapidjson::Value& params, JsonParsor& result);
	void moveFile(const rapidjson::Value& params, JsonParsor& result);
	void renameFile(const rapidjson::Value& params, JsonParsor& result);
	void watch(const rapidjson::Value& params, JsonParsor& result);
private:
};

