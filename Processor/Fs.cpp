#include <pch.h>
#include "Fs.h"
#include "../App/App.h"
#include "../Win/BrowserWindow.h"
#include "../Win/BrowserWindowConfig.h"
#include "../Win/Page.h"

namespace {
    std::unique_ptr<Fs> fs;
}

Fs::Fs()
{
}

Fs::~Fs()
{
}

Fs* Fs::get()
{
    if(!fs) {
        fs = std::make_unique<Fs>();
	}
    return fs.get();
}

void Fs::getFileInfo(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring path = Util::convertToWStr(arr[0].GetString());
    WIN32_FILE_ATTRIBUTE_DATA fileData;
    if (!GetFileAttributesEx(path.c_str(), GetFileExInfoStandard, &fileData)) {
        DWORD error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND) {
            result->addErr("file not found"); 
        }
        else {
            result->addErr("access error: " + std::to_string(error));
        }        
    }
    else if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        result->addErr("path is directory not file");
    }
    else {
        long long fileSize = static_cast<uint64_t>(fileData.nFileSizeHigh) << 32 | fileData.nFileSizeLow;
        result->addNumber("fileSize", fileSize);

        const uint64_t EPOCH_OFFSET_MS = 11644473600000ULL;
        ULARGE_INTEGER createTime;
        createTime.LowPart = fileData.ftCreationTime.dwLowDateTime;
        createTime.HighPart = fileData.ftCreationTime.dwHighDateTime;
        long long ct = (createTime.QuadPart / 10000) - EPOCH_OFFSET_MS;
        result->addNumber("createTime", ct);

        ULARGE_INTEGER lastWriteTime;
        lastWriteTime.LowPart = fileData.ftLastWriteTime.dwLowDateTime;
        lastWriteTime.HighPart = fileData.ftLastWriteTime.dwHighDateTime;
        long long lwt = (lastWriteTime.QuadPart / 10000) - EPOCH_OFFSET_MS;        
        result->addNumber("lastWriteTime", lwt);

        ULARGE_INTEGER lastAccessTime;
        lastAccessTime.LowPart = fileData.ftLastAccessTime.dwLowDateTime;
        lastAccessTime.HighPart = fileData.ftLastAccessTime.dwHighDateTime;
        long long lat = (lastAccessTime.QuadPart / 10000) - EPOCH_OFFSET_MS;
        result->addNumber("lastAccessTime", lat);

        auto isReadOnly = (fileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
        result->addBool("isReadOnly", isReadOnly);
        auto isHidden = (fileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0;
        result->addBool("isHidden", isHidden);
        auto isSystem = (fileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0;
        result->addBool("isSystem", isSystem);
        result->returnBack();
    }
}

void Fs::exists(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring path = Util::convertToWStr(arr[0].GetString());
    DWORD attributes = GetFileAttributes(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) { //无法获取文件或目录的属性
        result->addBool("isExists", false);
        DWORD error = GetLastError();
        if (error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND) {
            result->addErr("access error: " + std::to_string(error));
        }
    }
    else if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
        result->addBool("isExists", true);
        result->addBool("isDir", true);
    }
    else {
        result->addBool("isExists", true);
        result->addBool("isDir", false);
    }
    result->returnBack();
}

void Fs::readFile(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring filePath = Util::convertToWStr(arr[0].GetString());
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        result->addErr("Failed to open file.");
        return;
    }
    std::streamsize totalSize = file.tellg();
    file.seekg(0, std::ios::beg);
    if (file.fail()) {
        result->addErr("Failed to seek file.");
        return;
    }
    std::vector<char> buffer(totalSize);
    file.read(buffer.data(), totalSize);
    if (file.fail() && !file.eof()) {
        result->addErr("Failed to read file.");
        return;
    }
    file.close();
    auto env12 = App::get()->env.try_query<ICoreWebView2Environment12>();
    wil::com_ptr<ICoreWebView2SharedBuffer> sharedBuffer;
    auto hr = env12->CreateSharedBuffer(totalSize, &sharedBuffer);
    wil::com_ptr<IStream> stream;
    sharedBuffer->OpenStream(&stream);
    stream->Write(buffer.data(), totalSize, nullptr);
    result->addNumber("totalSize", totalSize);
    result->sharedBuffer = sharedBuffer.get();
    result->returnBackSharedBuffer();
    //关闭 C++ 端对共享缓冲区的访问权限。 通知操作系统，主进程不再持有该共享内存的引用。
    //此时共享内存由渲染进程持有，不会立即销毁。
    //渲染进程通过 chrome.webview.releaseBuffer 释放缓冲区。
    sharedBuffer->Close();
}

void Fs::readFileChunk(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring filePath = Util::convertToWStr(arr[0].GetString());

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        result->addErr("Failed to open file.");
        return;
    }
    std::streamsize start = arr[1].GetInt64();
    std::streamsize chunkSize = arr[2].GetInt64();
    std::streamsize totalSize = file.tellg();
    std::streamsize readSize = std::min(chunkSize, totalSize - start);
    file.seekg(start);
    if (file.fail()) {
        result->addErr("Failed to seek file.");
        return;
    }
    std::vector<char> buffer(readSize);
    file.read(buffer.data(), readSize);
    if (file.fail() && !file.eof()) {
        result->addErr("Failed to read file.");
        return;
    }
    file.close();

    auto env12 = App::get()->env.try_query<ICoreWebView2Environment12>();
    wil::com_ptr<ICoreWebView2SharedBuffer> sharedBuffer;
    auto hr = env12->CreateSharedBuffer(readSize, &sharedBuffer);
    wil::com_ptr<IStream> stream;
    sharedBuffer->OpenStream(&stream);    
    stream->Write(buffer.data(), readSize, nullptr);
    result->addNumber("totalSize", totalSize);
    result->addNumber("readSize", readSize);
    result->addNumber("startPos", start);
    result->sharedBuffer = sharedBuffer.get();
    result->returnBackSharedBuffer();
    //关闭 C++ 端对共享缓冲区的访问权限。 通知操作系统，主进程不再持有该共享内存的引用。
    //此时共享内存由渲染进程持有，不会立即销毁。
    //渲染进程通过 chrome.webview.releaseBuffer 释放缓冲区。
    sharedBuffer->Close();

}

void Fs::writeFile(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring filePath = Util::convertToWStr(arr[0].GetString());
    std::string fileContent = arr[1].GetString();
    std::ofstream file(filePath, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        result->addErr("Failed to open file.");
        return;
    }
    file.write(fileContent.data(), fileContent.size());
    file.flush();
    if (file.fail()) {
        result->addErr("Failed to write file.");
        return;
    }
    file.close();
    result->returnBack();
}

void Fs::writeFileChunk(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring filePath = Util::convertToWStr(arr[0].GetString());
    std::string fileContent = arr[1].GetString();
    std::streamsize startPos = arr[2].GetInt64();

    HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        result->addErr("Failed to open file.");
        return;
    }
    if (startPos == -1) {
        SetFilePointer(hFile, 0, NULL, FILE_END);
        DWORD bytesWritten;
        BOOL result = WriteFile(hFile, fileContent.c_str(), fileContent.size(), &bytesWritten,NULL);
        CloseHandle(hFile);
        return;
    }
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize)) {
        CloseHandle(hFile);
        result->addErr("can not get file size.");
        return;
    }
    if (startPos < 0 || startPos > fileSize.QuadPart) {
        CloseHandle(hFile);
        result->addErr("startPos > file size.");
        return;
    }
    HANDLE hMap = CreateFileMapping(hFile,NULL,
        PAGE_READWRITE, 0, 0, NULL ); //创建文件映射（映射整个文件）
    if (hMap == NULL) {
        CloseHandle(hFile);
        result->addErr("create file map error.");
        return;
    }
    LPVOID pMappedData = MapViewOfFile(hMap, 
        FILE_MAP_ALL_ACCESS, 0, 0, 0); //映射文件到内存
    if (pMappedData == NULL) {
        CloseHandle(hMap);
        CloseHandle(hFile);
        result->addErr("map view error.");
        return;
    }
    LARGE_INTEGER newSize; // 计算插入后文件大小
    newSize.QuadPart = fileSize.QuadPart + fileContent.size();
    // 调整文件大小（扩展文件）
    if (!SetFilePointerEx(hFile, newSize, NULL, FILE_BEGIN) ||
        !SetEndOfFile(hFile)) {
        UnmapViewOfFile(pMappedData);
        CloseHandle(hMap);
        CloseHandle(hFile);
        result->addErr("resize file error.");
        return;
    }

    //重新映射文件（现在文件已扩展）
    UnmapViewOfFile(pMappedData);
    pMappedData = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (pMappedData == NULL) {
        CloseHandle(hMap);
        CloseHandle(hFile);
        result->addErr("remap view error.");
        return;
    }
    // 移动插入位置后的数据（腾出空间）
    LPVOID pInsertPos = (LPVOID)((BYTE*)pMappedData + startPos);
    memmove((BYTE*)pInsertPos + fileContent.size(), pInsertPos, fileSize.QuadPart - startPos);    
    // 写入新文本
    memcpy(pInsertPos, fileContent.c_str(), fileContent.size());
    UnmapViewOfFile(pMappedData);
    CloseHandle(hMap);
    CloseHandle(hFile);
    result->returnBack();
}

void Fs::delPath(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring path = Util::convertToWStr(arr[0].GetString());
    DWORD fileAttributes = GetFileAttributes(path.c_str());
    if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
        result->addErr("access path error.");
        return;
    }
    if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        if (delDirRecursive(path)) return;
        result->addErr("recursive del error.");
        return;
    }
    if (!DeleteFile(path.c_str())) {
        result->addErr("del file error.");
    }
    result->returnBack();
}

void Fs::removePath(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring path = Util::convertToWStr(arr[0].GetString());
    DWORD fileAttributes = GetFileAttributes(path.c_str());
    if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
        result->addErr("access path error.");
        return;
    }
    SHFILEOPSTRUCT fileOp = { 0 };
    fileOp.hwnd = NULL;
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = path.c_str();
    fileOp.pTo = NULL;
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    int val = SHFileOperation(&fileOp);
    if (val != 0 || fileOp.fAnyOperationsAborted != FALSE) {
        result->addErr("remove path error.");
    }
    result->returnBack();
}

void Fs::createDir(const rapidjson::Value& params, JsonResult* result)
{
}

void Fs::listDir(const rapidjson::Value& params, JsonResult* result)
{
}

void Fs::copyFile(const rapidjson::Value& params, JsonResult* result)
{
}

void Fs::moveFile(const rapidjson::Value& params, JsonResult* result)
{
}

void Fs::renameFile(const rapidjson::Value& params, JsonResult* result)
{
}

void Fs::watch(const rapidjson::Value& params, JsonResult* result)
{
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
