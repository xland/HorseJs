#include <windows.h>
#include <shobjidl.h>

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

void Dialog::openPathDialog(BrowserWindow* win, const rapidjson::Value& params, JsonParsor& result)
{
    IFileOpenDialog* pFileOpen;
    auto hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));
    if (FAILED(hr)) {
        result.addString("err", "CoCreateInstance false");
        return;
    }
    DWORD dwOptions;
    hr = pFileOpen->GetOptions(&dwOptions);
    if (FAILED(hr)) {
        pFileOpen->Release();
        result.addString("err", "pFileOpen GetOptions err");
        return;
    }
    hr = pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);
    if (FAILED(hr)) {
        pFileOpen->Release();
        result.addString("err", "pFileOpen SetOptions err");
        return;
    }
    hr = pFileOpen->Show(NULL);
    if (FAILED(hr)) {
        pFileOpen->Release();
        result.addString("err", "pFileOpen Show err");
        return;
    }
    IShellItem* pItem;
    hr = pFileOpen->GetResult(&pItem);
    if (FAILED(hr)) {
        pFileOpen->Release();
        result.addString("err", "pFileOpen GetResult err");
        return;
    }
    PWSTR pszFolderPath;
    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
    if (FAILED(hr)) {
        CoTaskMemFree(pszFolderPath);
        pFileOpen->Release();
        result.addString("err", "GetDisplayName err");
        return;
    }
    CoTaskMemFree(pszFolderPath);
    pItem->Release();
    pFileOpen->Release();        
}
