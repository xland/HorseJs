#include <pch.h>
#include "Fs.h"

void Fs::enumFiles(const std::wstring& baseDir, const std::wstring& currentDir, std::vector<std::wstring>& fileList)
{
    std::wstring searchPath = currentDir + L"\\*";
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        const std::wstring name = findData.cFileName;
        if (name == L"." || name == L"..") continue;

        std::wstring fullPath = currentDir + L"\\" + name;
        std::wstring relativePath = fullPath.substr(baseDir.length() + 1);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            enumFiles(baseDir, fullPath, fileList);
        }
        else {
            fileList.emplace_back(relativePath);
        }
    } while (FindNextFile(hFind, &findData));
    FindClose(hFind);


    //std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    //std::streamsize size = file.tellg();
    //file.seekg(0, std::ios::beg);

    //std::vector<char> buffer(size);
    //file.read(buffer.data(), size);
}

bool Fs::addResToExe(const HANDLE& handle, std::wstring& resName, const std::wstring& resDir)
{
	auto filePath = resDir + L"\\" + resName;
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    file.read(buffer.data(), size);
	file.close();
    std::replace(resName.begin(), resName.end(), L'\\', L'/');
    BOOL ok = UpdateResource(handle,RT_RCDATA,resName.c_str(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (void*)buffer.data(), (DWORD)buffer.size());
    if (!ok) {
        EndUpdateResource(handle, TRUE);
        return false;
    }
    return true;
}

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