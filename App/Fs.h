#pragma once
#include "Util.h"

class Fs
{
public:
	Fs();
	~Fs();
	static void init();
	static Fs* get();
	void addDirAsExeRes(const std::wstring& dirPath,const std::wstring& exePath);
	void addFileAsExeRes();
	void addExeRes();
private:
};

