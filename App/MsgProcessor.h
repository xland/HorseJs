#pragma once
#include "Util.h"
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

