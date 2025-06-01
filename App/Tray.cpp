#include <pch.h>
#include "Tray.h"

namespace {
    std::unique_ptr<Tray> tray;
}

Tray::Tray()
{
}

Tray::~Tray()
{
}

Tray* Tray::get()
{
    if(!tray) {
        tray = std::make_unique<Tray>();
	}
    return tray.get();
}
