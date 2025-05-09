#include "App.h"
#include "../Win/WinBase.h"

void App::init()
{
    auto content = Util::readFile(L"config.json");
    rapidjson::Document d;
    d.Parse(content.data());
    auto win = WinBase::create(d["window"]);

}
