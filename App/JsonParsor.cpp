#include "JsonParsor.h"
#include "Util.h"

JsonParsor::JsonParsor() :allocator{ doc.GetAllocator() }
{
	doc.SetObject();
}

JsonParsor::~JsonParsor()
{
}

rapidjson::Document::AllocatorType& JsonParsor::getAllocator()
{
    return allocator;
}

void JsonParsor::addString(const std::string& name, const std::string& value)
{
    rapidjson::Value val;
    val.SetString(value.data(), value.length(), allocator);
    doc.AddMember(getKey(name), val, allocator);
}

void JsonParsor::addNumber(const std::string& name, const int& value)
{

    rapidjson::Value val;
    val.SetInt(value);
    doc.AddMember(getKey(name), val, allocator);
}

void JsonParsor::addNumber(const std::string& name, const double& value)
{
    rapidjson::Value val;
    val.SetDouble(value);
    doc.AddMember(getKey(name), val, allocator);
}

void JsonParsor::addNumber(const std::string& name, const long long& value)
{
    rapidjson::Value val;
    val.SetInt64(value);
    doc.AddMember(getKey(name), val, allocator);
}

void JsonParsor::addBool(const std::string& name, const bool& value)
{
    rapidjson::Value val;
    val.SetBool(value);
    doc.AddMember(getKey(name), val, allocator);
}

void JsonParsor::addValue(const std::string& name, rapidjson::Value&& value)
{
    rapidjson::Value key;
    key.SetString(name.data(), name.length(), allocator);
    doc.AddMember(key, std::move(value), allocator);
}

void JsonParsor::addParsor(const std::string& name, const JsonParsor&& parsor)
{
    rapidjson::Value copiedValue(parsor.doc, allocator);
    doc.AddMember(getKey(name), std::move(copiedValue), allocator);
}

std::wstring JsonParsor::parse()
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    std::string jsonStr = buffer.GetString();    
    return Util::convertToWStr(jsonStr.data());
}

rapidjson::Value JsonParsor::getKey(const std::string& name)
{
    rapidjson::Value key;
    key.SetString(name.data(), name.length(), allocator);
    return key;
}
