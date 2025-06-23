#include <pch.h>
#include "Horse.h"


void Horse::enumFiles(const std::wstring& baseDir, const std::wstring& currentDir, std::vector<std::wstring>& fileList)
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

bool Horse::addResToExe(const HANDLE& handle, std::wstring& resName, const std::wstring& resDir)
{
    auto filePath = resDir + L"\\" + resName;
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    file.read(buffer.data(), size);
    file.close();
    std::replace(resName.begin(), resName.end(), L'\\', L'/');
    BOOL ok = UpdateResource(handle, RT_RCDATA, resName.c_str(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (void*)buffer.data(), (DWORD)buffer.size());
    if (!ok) {
        EndUpdateResource(handle, TRUE);
        return false;
    }
    return true;
}