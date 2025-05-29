#include <future>
#include "Fs.h"
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
    std::async(std::launch::async, [win, &params, &result]() {
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        json response;
        if (!file.is_open()) {
            response = { {"callbackId", callbackId}, {"error", "Failed to open file"} };
        }
        else {
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
            std::vector<char> buffer(size);
            file.read(buffer.data(), size);
            file.close();
            std::string content(buffer.begin(), buffer.end());
            response = { {"callbackId", callbackId}, {"content", content} };
        }
        win->page->webview->PostWebMessageAsJson(std::wstring(response.dump().begin(), response.dump().end()).c_str());
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
