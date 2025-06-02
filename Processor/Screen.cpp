#include <pch.h>
#include "Screen.h"

namespace {
    std::unique_ptr<Screen> screen;
}

Screen::Screen()
{
}

Screen::~Screen()
{
}

Screen* Screen::get()
{
    if(!screen) {
        screen = std::make_unique<Screen>();
	}
    return screen.get();
}

//获取屏幕上某个点的颜色
//获取分辨率和DPI
//