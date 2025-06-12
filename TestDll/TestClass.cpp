#include "pch.h"
#include <string>
#include "TestClass.h"

const char* AddNumbers(const char* param) {
    std::string str{ param };
    str += "allen";
    return param;
}
