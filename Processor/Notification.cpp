#include <pch.h>
#include "Notification.h"

namespace {
    std::unique_ptr<Notification> notification;
}

Notification::Notification()
{
}

Notification::~Notification()
{
}

Notification* Notification::get()
{
    if(!notification) {
        notification = std::make_unique<Notification>();
	}
    return notification.get();
}
