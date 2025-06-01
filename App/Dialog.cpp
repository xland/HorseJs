#include <pch.h>

#include "Dialog.h"
#include "../Win/BrowserWindow.h"
#include "../Win/BrowserWindowConfig.h"
#include "../Win/Page.h"

namespace {
    std::unique_ptr<Dialog> dialog;
}

Dialog::Dialog()
{
}

Dialog::~Dialog()
{
}

Dialog* Dialog::get()
{
    if(!dialog) {
        dialog = std::make_unique<Dialog>();
	}
    return dialog.get();
}

void Dialog::openPathDialog(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto flag = arr[0].IsObject();
    const rapidjson::Value& obj = arr[0].GetObj();

    std::wstring title;
    if (obj.HasMember("title") && obj["title"].IsString()) {
        title = Util::convertToWStr(obj["title"].GetString());
    }
    std::jthread worker([result,title = std::move(title)]() {
        IFileOpenDialog* pFileOpen;
        auto hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));
        if (FAILED(hr)) {
            result->addString("err", "CoCreateInstance false");
            return;
        }
        DWORD dwOptions;
        hr = pFileOpen->GetOptions(&dwOptions);
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addString("err", "pFileOpen GetOptions err");
            return;
        }
        hr = pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);  //FOS_FORCEFILESYSTEM | FOS_ALLOWMULTISELECT | FOS_FORCESHOWHIDDEN
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addString("err", "pFileOpen SetOptions err");
            return;
        }
        hr = pFileOpen->SetTitle(title.data());
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addString("err", "set title err");
            return;
        }
        hr = pFileOpen->SetOkButtonLabel(L"选择");
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addString("err", "set ok button lable err");
            return;
        }
        std::wstring defaultPath = L"C:\\Users\\liulun\\AppData\\Roaming\\HorseJs"; // 替换为你的默认路径
        IShellItem* pDefaultFolder = nullptr;
        hr = SHCreateItemFromParsingName(defaultPath.c_str(), nullptr, IID_PPV_ARGS(&pDefaultFolder));
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addString("err", "set default dir err");
            return;
        }
        hr = pFileOpen->SetDefaultFolder(pDefaultFolder);  //todo SetFolder
        pDefaultFolder->Release();
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addString("err", "set default dir err");
            return;
        }
        COMDLG_FILTERSPEC fileTypes[] = {
            { L"All Files", L"*.*" },           // 所有文件
            { L"Text Files", L"*.txt" },        // 文本文件
            { L"Image Files", L"*.jpg;*.png" }  // 图片文件
        };
        hr = pFileOpen->SetFileTypes(_countof(fileTypes), fileTypes);
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addString("err", "set filter err");
            return;
        }
        hr = pFileOpen->Show(result->win->hwnd);
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addString("err", "pFileOpen Show err");
            return;
        }
        IShellItem* pItem;
        hr = pFileOpen->GetResult(&pItem);
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addString("err", "pFileOpen GetResult err");
            return;
        }
        PWSTR pszFolderPath;
        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
        if (FAILED(hr)) {
            CoTaskMemFree(pszFolderPath);
            pFileOpen->Release();
            result->addString("err", "GetDisplayName err");
            return;
        }
        std::wstring pathStr(pszFolderPath);
        CoTaskMemFree(pszFolderPath);
        pItem->Release();
        pFileOpen->Release();
        auto str = Util::convertToStr(pathStr);
        result->addString("path", str);
        result->returnBack();
        
        });
    worker.detach();
          
}
