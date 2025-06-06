#pragma once  
#include "../App/JsonParsor.h"  

class BrowserWindow;
class JsonResult : public JsonParsor  
{  
    public:  
        ~JsonResult();
        JsonResult(const int& winId,const std::string& className,const std::string& eventName);
        void addErr(const std::string& value);
        void returnBackThread();
        void returnBack();
        void returnBackSharedBuffer();
        BrowserWindow* getTar();
    public:
        bool ok{ true },cancel{false};
        int winId, tarId{-1};
        ICoreWebView2SharedBuffer* sharedBuffer; //todo大部分result都不需要这个指针，导致多了8个字节
    private:
    private:
};
