#pragma once
#include <string>
#include "../Lib/rapidjson/rapidjson.h"
#include "../Lib/rapidjson/document.h"
#include "../Lib/rapidjson/writer.h"
#include "../Lib/rapidjson/stringbuffer.h"

class JsonParsor
{
	public:
		JsonParsor();
		~JsonParsor();
		void addString(const std::string& name, const std::string& value);
		void addValue(const std::string& name, rapidjson::Value&& value);
		std::wstring parse();
	protected:

	private:
		rapidjson::Document doc;
		rapidjson::Document::AllocatorType& allocator;
};

