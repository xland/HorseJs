#pragma once

typedef void (*EventCB)(const char* eventName,const char* eventData);
extern "C" {
    __declspec(dllexport) const char* joinStr(const char* param);
    //必须使用这个名字
    __declspec(dllexport) const char* regEvent(const char* eventName,EventCB eventCB);
}

