#pragma once

enum class ClassId {
	Sys,
	App,
	Window,
	Page,
};
enum class WindowMethodId {
	maximize,
	minimize,
	resize,
	move,
	regEvent, 
	unregEvent,
};
enum class WindowEventId {
	closing,
	sizing,
	sized,
};

