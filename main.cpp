#include <pch.h>
#include "App/App.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    auto hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if(FAILED(hr))
    {
        MessageBox(NULL, L"Failed to initialize COM library", L"Error", MB_OK | MB_ICONERROR);
        return -1;
	}
    App::init(hInstance);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    CoUninitialize();
    return 0;
}