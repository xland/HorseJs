#include <pch.h>
#include "Fs.h"

bool Fs::delDirRecursive(const std::wstring& dirPath)
{
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile((dirPath + L"\\*").c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return false; // 目录不存在或访问失败
    }
    do {
        const std::wstring name = findData.cFileName;
        if (name == L"." || name == L"..") {
            continue; // 跳过 "." 和 ".."
        }
        const std::wstring fullPath = dirPath + L"\\" + name;
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // 是子目录：递归删除
            if (!delDirRecursive(fullPath)) {
                FindClose(hFind);
                return false;
            }
        }
        else {
            // 是文件：直接删除
            if (!DeleteFile(fullPath.c_str())) {
                FindClose(hFind);
                return false;
            }
        }
    } while (FindNextFile(hFind, &findData) != 0);
    FindClose(hFind);
    // 删除空目录
    return RemoveDirectory(dirPath.c_str()) != 0;
}

void Fs::getKnownPath(const GUID& type, JsonResult* result)
{
    PWSTR path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(type, 0, nullptr, &path);
    if (SUCCEEDED(hr)) {
        std::wstring strW(path);
        auto str = Util::convertToStr(strW);
        CoTaskMemFree(path);
        result->addString("data", str);
    }
    else {
        result->addErr("get known folder err");
    }
}