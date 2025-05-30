#include "ThreadManager.h"

std::vector<HANDLE>  threads_;
std::mutex  mutex_;

DWORD WINAPI ThreadFunc(LPVOID lpParam) {
    // 线程逻辑（如调用 COM 对话框）
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        IFileDialog* pFileOpen;
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));
        if (SUCCEEDED(hr)) {
            hr = pFileOpen->Show(NULL); // 阻塞线程
            if (SUCCEEDED(hr)) {
                // 处理结果...
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
    return 0; // 线程退出码
}

HANDLE ThreadManager::CreateManagedThread(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter)
{
    HANDLE hThread = CreateThread(
        NULL,                   // 默认安全属性
        0,                      // 默认堆栈大小
        lpStartAddress,         // 线程函数
        lpParameter,            // 参数
        0,                      // 默认创建标志
        NULL                    // 不需要线程ID
    );

    if (hThread != NULL) {
        std::lock_guard<std::mutex> lock(mutex_);
        threads_.push_back(hThread);
    }
    return hThread;
}

void ThreadManager::WaitForAllThreads()
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (HANDLE hThread : threads_) {
        if (hThread != NULL) {
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
        }
    }
    threads_.clear();
}
