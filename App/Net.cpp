#include <pch.h>
#include "Net.h"

namespace {
    std::unique_ptr<Net> net;
}

Net::Net()
{
}

Net::~Net()
{
}

Net* Net::get()
{
    if(!net) {
        net = std::make_unique<Net>();
	}
    return net.get();
}
