#pragma once
#include "Util.h"
#include "JsonParsor.h"
class Clipboard
{
public:
	Clipboard();
	~Clipboard();
	static Clipboard* get();
	
	void readText(const rapidjson::Value& params, JsonParsor& result);
	void writeText(const rapidjson::Value& params, JsonParsor& result);
	void readImage(const rapidjson::Value& params, JsonParsor& result);
	void writeImage(const rapidjson::Value& params, JsonParsor& result);
	void readFile(const rapidjson::Value& params, JsonParsor& result);
	void writeFile(const rapidjson::Value& params, JsonParsor& result);
	void readHtml(const rapidjson::Value& params, JsonParsor& result);
	void writeHtml(const rapidjson::Value& params, JsonParsor& result);
	void readRtf(const rapidjson::Value& params, JsonParsor& result);
	void writeRtf(const rapidjson::Value& params, JsonParsor& result);
	void readBookmark(const rapidjson::Value& params, JsonParsor& result);
	void writeBookmark(const rapidjson::Value& params, JsonParsor& result);
	void clear(const rapidjson::Value& params, JsonParsor& result);

private:
};

