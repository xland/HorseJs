#pragma once

enum class ClassId {
	Sys,
	Fs,
	App,
	Window,
	Page,
};
enum class FsMethodId {
	addResToExe,
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

