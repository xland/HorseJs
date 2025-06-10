#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Clipboard
{
public:
	Clipboard();
	~Clipboard();
	static Clipboard* get();
	static bool execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);
	
	void getDataType(const rapidjson::Value& params, JsonResult* result);
	void readText(const rapidjson::Value& params, JsonResult* result);
	void writeText(const rapidjson::Value& params, JsonResult* result);
	void readImg1(const rapidjson::Value& params, JsonResult* result);
	void readImg(const rapidjson::Value& params, JsonResult* result);
	void writeImg(const rapidjson::Value& params, JsonResult* result);
	void getFile(const rapidjson::Value& params, JsonResult* result);
	void addFile(const rapidjson::Value& params, JsonResult* result);
	void readHtml(const rapidjson::Value& params, JsonResult* result);
	void writeHtml(const rapidjson::Value& params, JsonResult* result);
	void readRtf(const rapidjson::Value& params, JsonResult* result);
	void writeRtf(const rapidjson::Value& params, JsonResult* result);
	void clear(const rapidjson::Value& params, JsonResult* result);

private:
};

