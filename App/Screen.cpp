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
