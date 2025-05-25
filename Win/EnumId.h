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
	close,
	destory,
	regEvent, 
	unregEvent,
};
enum class WindowEventId {
	closing
};

