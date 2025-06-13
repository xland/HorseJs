#include "pch.h"
#include <combaseapi.h>
#include <string>
#include <thread>
#include <chrono>
#include "TestClass.h"

static EventCB _eventCB{nullptr};

const char* joinStr(const char* param) {
    std::string str{ param };
    str += "world!";
    //必须使用这种方式分配返回值的内存，这样HorseJs才能释放返回值的内存
    char* dynamicStr = (char*)CoTaskMemAlloc(str.size() + 1); 
    if (dynamicStr) {
        strcpy_s(dynamicStr, str.size() + 1, str.c_str());
        return dynamicStr;
    }
    return nullptr;
}

const char* regEvent(const char* eventName, EventCB eventCB)
{
    _eventCB = eventCB;
    std::string eName(eventName);
    std::string tarName("customEvent");
    if (eName == tarName) {
        //99%的情况下，事件都是在新线程内执行的
        std::jthread worker{ []() {
            for (size_t i = 0; i < 88; i++)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                //参数占用的内存，需要Dll开发者负责释放
                _eventCB("customEvent", "customEvent data"); 
            }
        } };
        worker.detach();
    }
    return "ok";
}

const char* unregEvent(const char* eventName)
{
    //dll 开发者负责停止event线程
    return nullptr;
}
