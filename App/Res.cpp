#include "Res.h"

namespace {
    std::unique_ptr<Res> res;
}

Res::Res()
{
}

Res::~Res()
{
}

Res* Res::get()
{
    if(!res) {
        res = std::make_unique<Res>();
	}
    return res.get();
}
