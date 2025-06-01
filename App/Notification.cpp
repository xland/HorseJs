#include "Os.h"

namespace {
    std::unique_ptr<Os> os;
}

Os::Os()
{
}

Os::~Os()
{
}

Os* Os::get()
{
    if(!os) {
        os = std::make_unique<Os>();
	}
    return os.get();
}
