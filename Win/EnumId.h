#pragma once

enum class ClassId {
	Sys,
	Fs,
	Horse,
	Window,
	Page,
};
enum class FsMethodId {
	addResToExe,
};

enum class HorseMethodId {
	getConfig,
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
	flash,
	startDrag,
	setResizable,
	regEvent, 
	unregEvent,
};
enum class WindowEventId {
	closing,
	sizePosChanged,
	stateChanged
};

