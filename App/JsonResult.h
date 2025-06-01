#pragma once  
#include "JsonParsor.h"  

class JsonResult : public JsonParsor  
{  
    public:  
        JsonResult();  
        ~JsonResult();
        void addErr(const std::string& value);

    public:
        bool ok{ true };
        bool isAsync{ false };
};
