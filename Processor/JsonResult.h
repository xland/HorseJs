#pragma once  
#include "../App/JsonParsor.h"  

class BrowserWindow;
class JsonResult : public JsonParsor  
{  
    public:  
        ~JsonResult();
        static JsonResult* create(BrowserWindow* win, BrowserWindow* tar, std::string& className,std::string& eventName);
        void addErr(const std::string& value);
        void returnBackThread();
        void returnBack(bool delSelf = true);
        void returnBackSharedBuffer();
    public:
        bool ok{ true };
		std::string className,eventName;
        BrowserWindow* win;
        BrowserWindow* tar;
        ICoreWebView2SharedBuffer* sharedBuffer; //todo大部分result都不需要这个指针，导致多了8个字节
    private:
        JsonResult(BrowserWindow* win, BrowserWindow* tar, std::string& className, std::string& eventName);
    private:
};
