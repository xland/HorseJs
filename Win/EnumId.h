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
};
enum class WindowEventId {
	closing,
	closed,
};

