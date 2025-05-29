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
    //todo 判断文件是否存在，不存在通过result返回err
    std::string eventName = arr[1].GetString();
    std::async(std::launch::async, [win, &filePath,&eventName]() {
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            JsonParsor parsor;
            parsor.addString("className", "fs");
            parsor.addString("eventName", eventName);
            parsor.addString("err", "Failed to open file.");
            std::wstring jsonStr = parsor.parse();
            win->page->webview->PostWebMessageAsJson(jsonStr.data());
            return;
        }
        std::streamsize totalSize = file.tellg();
        file.seekg(0, std::ios::beg);
        const std::streamsize chunkSize = 8 * 1024 * 1024; // 8MB 每块
        std::vector<char> buffer(chunkSize);
        auto env12 = App::get()->env.try_query<ICoreWebView2Environment12>();
        auto webview17 = win->page->webview.try_query<ICoreWebView2_17>();
        for (std::streamsize offset = 0; offset < totalSize; offset += chunkSize) {
            std::streamsize sizeToRead = std::min(chunkSize, totalSize - offset);
            file.read(buffer.data(), sizeToRead);
            wil::com_ptr<ICoreWebView2SharedBuffer> sharedBuffer;
            env12->CreateSharedBuffer(sizeToRead, &sharedBuffer);
            wil::com_ptr<IStream> stream;
            sharedBuffer->OpenStream(&stream);
            stream->Write(buffer.data(), sizeToRead, nullptr);

            JsonParsor parsor;
            parsor.addString("className", "fs");
            parsor.addString("eventName", eventName);
            parsor.addNumber("offset", offset);
            parsor.addNumber("totalSize", totalSize);
            std::wstring jsonStr = parsor.parse();
            webview17->PostSharedBufferToScript(sharedBuffer.get(), COREWEBVIEW2_SHARED_BUFFER_ACCESS_READ_ONLY, jsonStr.data());
            //关闭 C++ 端对共享缓冲区的访问权限。 通知操作系统，主进程不再持有该共享内存的引用。
            //此时共享内存由渲染进程持有，不会立即销毁。
            //渲染进程通过 chrome.webview.releaseBuffer 释放缓冲区。
            sharedBuffer->Close(); //现在就Close是不是太早了？
        }
        file.close();
    });
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
