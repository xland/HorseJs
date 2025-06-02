
#include <pch.h>
#include "Process.h"

namespace {
    std::unique_ptr<Process> process;
}

Process::Process()
{
}

Process::~Process()
{
}

Process* Process::get()
{
    if(!process) {
        process = std::make_unique<Process>();
	}
    return process.get();
}
