#include <future>
#include "Fs.h"
#include "Util.h"
#include "App.h"
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

void Fs::getFileInfo(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring path = Util::convertToWStr(arr[0].GetString());
    WIN32_FILE_ATTRIBUTE_DATA fileData;
    if (!GetFileAttributesEx(path.c_str(), GetFileExInfoStandard, &fileData)) {
        DWORD error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND) {
            result.addString("err", "file not found");
        }
        else {
            result.addString("err", "access error: " + std::to_string(error));
        }
    }
    else if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        result.addString("err", "path is directory not file");
    }
    else {
        long long fileSize = static_cast<uint64_t>(fileData.nFileSizeHigh) << 32 | fileData.nFileSizeLow;
        result.addNumber("fileSize", fileSize);

        const uint64_t EPOCH_OFFSET_MS = 11644473600000ULL;
        ULARGE_INTEGER createTime;
        createTime.LowPart = fileData.ftCreationTime.dwLowDateTime;
        createTime.HighPart = fileData.ftCreationTime.dwHighDateTime;
        long long ct = (createTime.QuadPart / 10000) - EPOCH_OFFSET_MS;
        result.addNumber("createTime", ct);

        ULARGE_INTEGER lastWriteTime;
        lastWriteTime.LowPart = fileData.ftLastWriteTime.dwLowDateTime;
        lastWriteTime.HighPart = fileData.ftLastWriteTime.dwHighDateTime;
        long long lwt = (lastWriteTime.QuadPart / 10000) - EPOCH_OFFSET_MS;        
        result.addNumber("lastWriteTime", lwt);

        ULARGE_INTEGER lastAccessTime;
        lastAccessTime.LowPart = fileData.ftLastAccessTime.dwLowDateTime;
        lastAccessTime.HighPart = fileData.ftLastAccessTime.dwHighDateTime;
        long long lat = (lastAccessTime.QuadPart / 10000) - EPOCH_OFFSET_MS;
        result.addNumber("lastAccessTime", lat);

        auto isReadOnly = (fileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
        result.addBool("isReadOnly", isReadOnly);
        auto isHidden = (fileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0;
        result.addBool("isHidden", isHidden);
        auto isSystem = (fileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0;
        result.addBool("isSystem", isSystem);
    }
}

void Fs::exists(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring path = Util::convertToWStr(arr[0].GetString());
    DWORD attributes = GetFileAttributes(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) { //无法获取文件或目录的属性
        result.addBool("isExists", false);
        DWORD error = GetLastError();
        if (error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND) {
            result.addString("err", "access error: " + std::to_string(error));
        }
    }
    else if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
        result.addBool("isExists", true);
        result.addBool("isDir", true);
    }
    else {
        result.addBool("isExists", true);
        result.addBool("isDir", false);
    }
}

void Fs::readFile(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring filePath = Util::convertToWStr(arr[0].GetString());
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        result.addString("err", "Failed to open file.");
        return;
    }
    std::streamsize totalSize = file.tellg();
    file.seekg(0, std::ios::beg);
    if (file.fail()) {
        result.addString("err", "Failed to seek file.");
        return;
    }
    std::vector<char> buffer(totalSize);
    file.read(buffer.data(), totalSize);
    if (file.fail() && !file.eof()) {
        result.addString("err", "Failed to read file.");
        return;
    }
    file.close();
    auto env12 = App::get()->env.try_query<ICoreWebView2Environment12>();
    wil::com_ptr<ICoreWebView2SharedBuffer> sharedBuffer;
    auto hr = env12->CreateSharedBuffer(totalSize, &sharedBuffer);
    wil::com_ptr<IStream> stream;
    sharedBuffer->OpenStream(&stream);
    stream->Write(buffer.data(), totalSize, nullptr);
    result.isSharedBuffer = true;
    result.addNumber("totalSize", totalSize);
    std::wstring jsonStr = result.parse();
    auto webview17 = win->page->webview.try_query<ICoreWebView2_17>();
    webview17->PostSharedBufferToScript(sharedBuffer.get(), COREWEBVIEW2_SHARED_BUFFER_ACCESS_READ_ONLY, jsonStr.data());
    //关闭 C++ 端对共享缓冲区的访问权限。 通知操作系统，主进程不再持有该共享内存的引用。
    //此时共享内存由渲染进程持有，不会立即销毁。
    //渲染进程通过 chrome.webview.releaseBuffer 释放缓冲区。
    sharedBuffer->Close();
}

void Fs::readFileChunk(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring filePath = Util::convertToWStr(arr[0].GetString());

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        result.addString("err", "Failed to open file.");
        return;
    }
    std::streamsize start = arr[1].GetInt64();
    std::streamsize chunkSize = arr[2].GetInt64();
    std::streamsize totalSize = file.tellg();
    std::streamsize readSize = std::min(chunkSize, totalSize - start);
    file.seekg(start);
    if (file.fail()) {
        result.addString("err", "Failed to seek file.");
        return;
    }
    std::vector<char> buffer(readSize);
    file.read(buffer.data(), readSize);
    if (file.fail() && !file.eof()) {
        result.addString("err", "Failed to read file.");
        return;
    }
    file.close();

    auto env12 = App::get()->env.try_query<ICoreWebView2Environment12>();
    wil::com_ptr<ICoreWebView2SharedBuffer> sharedBuffer;
    auto hr = env12->CreateSharedBuffer(readSize, &sharedBuffer);
    wil::com_ptr<IStream> stream;
    sharedBuffer->OpenStream(&stream);    
    stream->Write(buffer.data(), readSize, nullptr);
    result.isSharedBuffer = true;
    result.addNumber("totalSize", totalSize);
    result.addNumber("readSize", readSize);
    result.addNumber("startPos", start);
    std::wstring jsonStr = result.parse();
    auto webview17 = win->page->webview.try_query<ICoreWebView2_17>();
    webview17->PostSharedBufferToScript(sharedBuffer.get(), COREWEBVIEW2_SHARED_BUFFER_ACCESS_READ_ONLY, jsonStr.data());
    //关闭 C++ 端对共享缓冲区的访问权限。 通知操作系统，主进程不再持有该共享内存的引用。
    //此时共享内存由渲染进程持有，不会立即销毁。
    //渲染进程通过 chrome.webview.releaseBuffer 释放缓冲区。
    sharedBuffer->Close();
}

void Fs::writeFile(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result)
{
}

void Fs::removeFile(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result)
{
}

void Fs::removeDir(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result)
{
}

void Fs::createDir(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result)
{
}

void Fs::listDir(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result)
{
}

void Fs::copyFile(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result)
{
}

void Fs::moveFile(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result)
{
}

void Fs::renameFile(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result)
{
}

void Fs::watch(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result)
{
}
