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
	hide,
	show,
	restore,
	resize,
	move,
	regEvent, 
	unregEvent,
	close,
	destory,
};
enum class WindowEventId {
	closing
};

