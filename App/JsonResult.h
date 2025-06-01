#pragma once  
#include "JsonParsor.h"  

class BrowserWindow;
class JsonResult : public JsonParsor  
{  
    public:  
        ~JsonResult();
        static JsonResult* create(BrowserWindow* win,std::string& className,std::string& eventName);
        void addErr(const std::string& value);
        void returnBack();
        void returnBackSharedBuffer();
    public:
        bool ok{ true };
        BrowserWindow* win;
        ICoreWebView2SharedBuffer* sharedBuffer; //todo大部分result都不需要这个指针，导致多了8个字节
    private:
        JsonResult(BrowserWindow* win);
    private:
};
