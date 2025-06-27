#pragma once
#include "../App/Util.h"
#include "JsonResult.h"
class Db
{
public:
	Db();
	~Db();
	static Db* get();
	static bool execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result);

	void open(const rapidjson::Value& params, JsonResult* result);
	void prepare(const rapidjson::Value& params, JsonResult* result);
	void exec(const rapidjson::Value& params, JsonResult* result);
	void close(const rapidjson::Value& params, JsonResult* result);
	void del(const rapidjson::Value& params, JsonResult* result);
private:
	static int callback(void* NotUsed, int argc, char** argv, char** azColName);
};

