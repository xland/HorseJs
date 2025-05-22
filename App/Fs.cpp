#include <iostream>
#include <filesystem>
#include <regex>
#include "Fs.h"

namespace {
    std::unique_ptr<Fs> fs;
}

Fs::Fs()
{
}

Fs::~Fs()
{
}

void Fs::init()
{
    fs = std::make_unique<Fs>();
}

Fs* Fs::get()
{
    return fs.get();
}

void Fs::addDirAsExeRes(const std::wstring& dirPath, const std::wstring& exePath)
{
    std::filesystem::path root{ dirPath };
    if (dirPath.back() == L'\0') {
        auto str = dirPath;
        str.pop_back();
        root = std::filesystem::path(str);
    }
    if (!std::filesystem::is_directory(root)) {
        std::cerr << "不是一个有效的目录\n";
        return;
    }
    HANDLE hResource = BeginUpdateResource(exePath.data(), FALSE);
    try {
        for (auto const& entry : std::filesystem::recursive_directory_iterator(root)) {
            if (entry.is_regular_file()) {
                std::filesystem::path relativePath = std::filesystem::relative(entry.path(), root);  //  images/logo.png
                std::wstring resName = std::regex_replace(relativePath.wstring(), std::wregex(LR"(\\)"), L"/");
                std::uintmax_t fileSize = std::filesystem::file_size(entry.path());
                std::ifstream file(entry.path(), std::ios::binary);
                std::vector<char> buffer(fileSize);
                file.read(buffer.data(), fileSize);
                buffer.erase(std::remove_if(buffer.begin(), buffer.end(),[](char c) {
                            return c == ' ' || c == '\n' || c == '\r' || c == '\t';
                        }),buffer.end());
                auto langId = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);
                BOOL ok = UpdateResource(hResource, RT_RCDATA, resName.data(), langId,(void*)buffer.data(), (DWORD)fileSize);
                if (!ok) {
                    std::wcerr << L"UpdateResource failed\n";
                    EndUpdateResource(hResource, TRUE);
                    return;
                }
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "文件系统错误: " << e.what() << "\n";
        return;
    }
    if (!EndUpdateResourceW(hResource, FALSE)) {  // FALSE = 保存修改
        std::wcerr << L"EndUpdateResource failed\n";
        return;
    }
}

void Fs::addFileAsExeRes()
{

}

void Fs::addExeRes()
{

}
