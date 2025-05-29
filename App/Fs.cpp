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

void Fs::stat(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result)
{
}

void Fs::exists(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result)
{
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
    auto env12 = App::get()->env.try_query<ICoreWebView2Environment12>();
    wil::com_ptr<ICoreWebView2SharedBuffer> sharedBuffer;
    auto hr = env12->CreateSharedBuffer(totalSize, &sharedBuffer);
    wil::com_ptr<IStream> stream;
    sharedBuffer->OpenStream(&stream);
    std::vector<char> buffer(totalSize);
    file.read(buffer.data(), totalSize);
    file.close();
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
    std::vector<char> buffer(readSize);
    file.read(buffer.data(), readSize);
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
