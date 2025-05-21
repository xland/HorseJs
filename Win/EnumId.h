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
	close,
	destory,
};
enum class WindowEventId {
	closing,
	sized,
};

