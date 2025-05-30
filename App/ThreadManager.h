#pragma once
#include <windows.h>
#include <vector>
#include <mutex>
class ThreadManager
{
public:
    // 创建线程并自动管理
    static HANDLE CreateManagedThread(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter);
    static void WaitForAllThreads();
};

