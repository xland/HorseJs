#pragma once
#include "Util.h"
#include "JsonResult.h"
class Res
{
public:
	Res();
	~Res();
	static Res* get();

	void addDirAsExeRes(const std::wstring& dirPath, const std::wstring& exePath);
	void addFileAsExeRes();
	void addExeRes();
private:
};

