#include "JsonParsor.h"
#include "Util.h"

JsonParsor::JsonParsor() :allocator{ doc.GetAllocator() }
{
	doc.SetObject();
}

JsonParsor::~JsonParsor()
{
}

void JsonParsor::addString(const std::string& name, const std::string& value)
{
    rapidjson::Value key;
    key.SetString(name.data(), name.length(), allocator);

    rapidjson::Value val;
    val.SetString(value.data(), value.length(), allocator);

    doc.AddMember(key, val, allocator);
}

void JsonParsor::addValue(const std::string& name, rapidjson::Value&& value)
{
    rapidjson::Value key;
    key.SetString(name.data(), name.length(), allocator);
    doc.AddMember(key, std::move(value), allocator);
}

std::wstring JsonParsor::parse()
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    std::string jsonStr = buffer.GetString();    
    return Util::convertToWStr(jsonStr.data());
}
