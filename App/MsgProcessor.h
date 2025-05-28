#pragma once
#include <cstdarg>
#include "Util.h"

class BrowserWindow;
class Page;
class JsonParsor;
class MsgProcessor
{
public:
	MsgProcessor();
	~MsgProcessor();
	static MsgProcessor* get();
	void processStr(const std::string& msgStr);
public:
private:
};

