#include <pch.h>
#include "JsonParsor.h"
#include "Util.h"

JsonParsor::JsonParsor() 
{
	doc.SetObject();
}

JsonParsor::~JsonParsor()
{
}

rapidjson::Document::AllocatorType& JsonParsor::getAllocator()
{
    return doc.GetAllocator();
}

void JsonParsor::addString(const std::string& name, const std::string& value)
{
    rapidjson::Value val;
    val.SetString(value.data(), (unsigned)value.length(), doc.GetAllocator());
    doc.AddMember(getKey(name), val, doc.GetAllocator());
}

std::string JsonParsor::getString(const std::string& name)
{
    return doc[name.data()].GetString();
}

rapidjson::Value JsonParsor::getVal()
{
    rapidjson::Value docValue;
    docValue.CopyFrom(doc,doc.GetAllocator());
    return docValue;
}

void JsonParsor::addNumber(const std::string& name, const int& value)
{

    rapidjson::Value val;
    val.SetInt(value);
    doc.AddMember(getKey(name), val, doc.GetAllocator());
}

void JsonParsor::addNumber(const std::string& name, const double& value)
{
    rapidjson::Value val;
    val.SetDouble(value);
    doc.AddMember(getKey(name), val, doc.GetAllocator());
}

void JsonParsor::addNumber(const std::string& name, const long long& value)
{
    rapidjson::Value val;
    val.SetInt64(value);
    doc.AddMember(getKey(name), val, doc.GetAllocator());
}

void JsonParsor::addBool(const std::string& name, const bool& value)
{
    rapidjson::Value val;
    val.SetBool(value);
    doc.AddMember(getKey(name), val, doc.GetAllocator());
}

void JsonParsor::addValue(const std::string& name, rapidjson::Value&& value)
{
    rapidjson::Value key;
    key.SetString(name.data(), name.length(), doc.GetAllocator());
    doc.AddMember(key, std::move(value), doc.GetAllocator());
}

void JsonParsor::addParsor(const std::string& name, const JsonParsor&& parsor)
{
    rapidjson::Value copiedValue(parsor.doc, doc.GetAllocator());
    doc.AddMember(getKey(name), std::move(copiedValue), doc.GetAllocator());
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
    key.SetString(name.data(), name.length(), doc.GetAllocator());
    return key;
}
