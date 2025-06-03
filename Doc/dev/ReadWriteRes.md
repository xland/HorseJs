```
HANDLE hFile;
DWORD dwFileSize,dwBytesRead;
LPBYTE lpBuffer;
char szFile[MAX_PATH+1] = {0};
::GetDlgItemText(hwnd,EditId,szFile,MAX_PATH);
hFile = CreateFile(szFile,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
dwFileSize = GetFileSize(hFile, NULL);
lpBuffer = new BYTE[dwFileSize];
ReadFile(hFile, lpBuffer, dwFileSize, &dwBytesRead, NULL);
HANDLE hResource = BeginUpdateResource(szFilePath, FALSE);
UpdateResource(hResource,RT_RCDATA,MAKEINTRESOURCE(EditId),MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),(LPVOID)lpBuffer,dwFileSize); 
EndUpdateResource(hResource, FALSE); 
delete [] lpBuffer; 
CloseHandle(hFile);
return 1;
```

```
#include <windows.h>
#include <iostream>

bool add_resource_to_exe(const std::wstring& exePath, const std::wstring& resName, const std::vector<uint8_t>& data) 
{
    HANDLE hUpdate = BeginUpdateResourceW(exePath.c_str(), FALSE);  // FALSE = 保留原资源
    if (!hUpdate) {
        std::wcerr << L"BeginUpdateResource failed\n";
        return false;
    }
    BOOL ok = UpdateResourceW(
        hUpdate,
        RT_RCDATA,                      // 资源类型
        resName.c_str(),               // 资源名称（字符串或整数 MAKEINTRESOURCE）
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        (void*)data.data(),
        (DWORD)data.size()
    );
    if (!ok) {
        std::wcerr << L"UpdateResource failed\n";
        EndUpdateResource(hUpdate, TRUE); // TRUE = 放弃修改
        return false;
    }
    if (!EndUpdateResourceW(hUpdate, FALSE)) {  // FALSE = 保存修改
        std::wcerr << L"EndUpdateResource failed\n";
        return false;
    }
    return true;
}
```

```
HMODULE hInstance = ::GetModuleHandle(NULL);
TCHAR  szFilePath[MAX_PATH + 1];
GetPath(szFilePath,resourceName,hInstance);
HRSRC hResID = ::FindResource(hInstance,resourceID,RT_RCDATA); 
HGLOBAL hRes = ::LoadResource(hInstance,hResID);
LPVOID pRes = ::LockResource(hRes);
DWORD dwResSize = ::SizeofResource(hInstance,hResID);
if(!dwResSize) return 0;
HANDLE hResFile = CreateFile(szFilePath,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);    
DWORD dwWritten = 0;
WriteFile(hResFile,pRes,dwResSize,&dwWritten,NULL);
CloseHandle(hResFile);
if(dwResSize == dwWritten) return 1;
return 0;
```

```
#include <windows.h>
#include <vector>
#include <string>
#include <stdexcept>
std::string path_to_resname(const std::string& path) {
    std::string res = path;
    for (char& c : res) {
        if (c == '/' || c == '\\') c = '_';
        else c = std::toupper(static_cast<unsigned char>(c));
    }
    return res;
}
std::vector<uint8_t> load_resource_by_path(const std::string& path) {
    std::string resname = path_to_resname(path);

    HMODULE hModule = GetModuleHandle(nullptr); // 当前 exe
    HRSRC hRes = FindResourceA(hModule, resname.c_str(), RT_RCDATA);
    if (!hRes) throw std::runtime_error("Resource not found: " + path);

    HGLOBAL hData = LoadResource(hModule, hRes);
    if (!hData) throw std::runtime_error("Failed to load resource: " + path);

    DWORD size = SizeofResource(hModule, hRes);
    void* pData = LockResource(hData);

    if (!pData) throw std::runtime_error("Failed to lock resource: " + path);

    return std::vector<uint8_t>((uint8_t*)pData, (uint8_t*)pData + size);
}
```