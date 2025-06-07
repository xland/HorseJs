#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Clipboard
{
public:
	Clipboard();
	~Clipboard();
	static Clipboard* get();
	static bool excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	
	void getDataType(const rapidjson::Value& params, JsonResult* result);
	void readText(const rapidjson::Value& params, JsonResult* result);
	void writeText(const rapidjson::Value& params, JsonResult* result);
	void readImage(const rapidjson::Value& params, JsonResult* result);
	void writeImage(const rapidjson::Value& params, JsonResult* result);
	void readFile(const rapidjson::Value& params, JsonResult* result);
	void writeFile(const rapidjson::Value& params, JsonResult* result);
	void readHtml(const rapidjson::Value& params, JsonResult* result);
	void writeHtml(const rapidjson::Value& params, JsonResult* result);
	void readRtf(const rapidjson::Value& params, JsonResult* result);
	void writeRtf(const rapidjson::Value& params, JsonResult* result);
	void readBookmark(const rapidjson::Value& params, JsonResult* result);
	void writeBookmark(const rapidjson::Value& params, JsonResult* result);
	void clear(const rapidjson::Value& params, JsonResult* result);

private:
};

