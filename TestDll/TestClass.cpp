#include "pch.h"
#include <combaseapi.h>
#include <string>
#include "TestClass.h"

const char* joinStr(const char* param) {
    std::string str{ param };
    str += "world!";
    char* dynamicStr = (char*)CoTaskMemAlloc(str.size() + 1);
    if (dynamicStr) {
        strcpy_s(dynamicStr, str.size() + 1, str.c_str());
        return dynamicStr;
    }
    return "err";
}
