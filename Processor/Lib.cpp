#include <pch.h>
#include "Lib.h"

namespace {
    std::unique_ptr<Lib> lib;
}

Lib::Lib()
{
}

Lib::~Lib()
{
}

Lib* Lib::get()
{
    if(!lib) {
        lib = std::make_unique<Lib>();
	}
    return lib.get();
}
