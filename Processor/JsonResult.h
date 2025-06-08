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
        void returnBackSharedBuffer(ICoreWebView2SharedBuffer* sharedBuffer);
        BrowserWindow* getTar();
        BrowserWindow* getWin();
    public:
        bool ok{ true },cancel{false};
        int winId, tarId{-1};
    private:
    private:
};
