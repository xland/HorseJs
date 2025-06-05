#include <pch.h>

#include "Dialog.h"
#include "../Win/BrowserWindow.h"
#include "../Win/BrowserWindowConfig.h"
#include "../Win/Page.h"

namespace {
    std::unique_ptr<Dialog> dialog;
    static std::unordered_map<std::string, void (Dialog::*)(const rapidjson::Value&, JsonResult*)> dialogFunc{
    {"openPathDialog", &Dialog::openPathDialog},
    };
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
bool Dialog::excute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = dialogFunc.find(methodName);
    if (it == dialogFunc.end()) return false;
    (Dialog::get()->*it->second)(param, result);
    return true;
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

    std::wstring okBtnText;
    if (obj.HasMember("okBtnText") && obj["okBtnText"].IsString()) {
        okBtnText = Util::convertToWStr(obj["okBtnText"].GetString());
    }

    std::wstring defaultDir;
    if (obj.HasMember("defaultDir") && obj["defaultDir"].IsString()) {
        defaultDir = Util::convertToWStr(obj["defaultDir"].GetString());
    }

    FILEOPENDIALOGOPTIONS option{ FOS_FORCEFILESYSTEM };
    std::string type{"file"};
    if (obj.HasMember("type") && obj["type"].IsString()) {
        type = obj["type"].GetString();
        if (type == "dir") {
            option = FOS_PICKFOLDERS;
        }
    }
    std::vector<std::pair<std::wstring, std::wstring>> filters;
    if (type == "file") {
        if (obj.HasMember("filter") && obj["filter"].IsArray()) {
            const rapidjson::Value::ConstArray arr = obj["filter"].GetArray();
            for (const auto& filter : arr) {
                const rapidjson::Value::ConstArray arr = filter.GetArray();
                auto name = Util::convertToWStr(arr[0].GetString());
                auto val = Util::convertToWStr(arr[1].GetString());
                filters.push_back({ name, val });
            }
        }
    }

    bool multiSelection{false};
    if (obj.HasMember("multiSelection") && obj["multiSelection"].IsBool()) {
        multiSelection = obj["multiSelection"].GetBool();
        if (multiSelection) {
            option |= FOS_ALLOWMULTISELECT;
        }
    }

    bool showHiddenFile{false};
    if (obj.HasMember("showHiddenFile") && obj["showHiddenFile"].IsBool()) {
        showHiddenFile = obj["showHiddenFile"].GetBool();
        if (showHiddenFile) {
            option |= FOS_FORCESHOWHIDDEN;
        }
    }

    std::jthread worker([result,
        title = std::move(title),
        okBtnText = std::move(okBtnText),
        defaultDir = std::move(defaultDir),
        option = std::move(option),
        filters = std::move(filters)
    ]() {
        IFileOpenDialog* pFileOpen;
        auto hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));
        if (FAILED(hr)) {
            result->addErr("CoCreateInstance false");
            return;
        }
        DWORD dwOptions;
        hr = pFileOpen->GetOptions(&dwOptions);
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addErr("pFileOpen GetOptions err");
            return;
        }
        hr = pFileOpen->SetOptions(dwOptions | option);
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addErr("pFileOpen SetOptions err");
            return;
        }
        if(!title.empty()) {
            hr = pFileOpen->SetTitle(title.data());
            if (FAILED(hr)) {
                pFileOpen->Release();
                result->addErr("set title err");
                return;
            }
		}
        if (!okBtnText.empty()) {
            hr = pFileOpen->SetOkButtonLabel(okBtnText.data());
            if (FAILED(hr)) {
                pFileOpen->Release();
                result->addErr("set ok button lable err");
                return;
            }
        }
        if (!defaultDir.empty()) {
            IShellItem* pDefaultFolder = nullptr;
            hr = SHCreateItemFromParsingName(defaultDir.c_str(), nullptr, IID_PPV_ARGS(&pDefaultFolder));
            if (FAILED(hr)) {
                pFileOpen->Release();
                result->addErr("set default dir err");
                return;
            }
            hr = pFileOpen->SetDefaultFolder(pDefaultFolder);  //todo SetFolder
            pDefaultFolder->Release();
            if (FAILED(hr)) {
                pFileOpen->Release();
                result->addErr("set default dir err");
                return;
            }
        }

        if (!filters.empty()) {
            std::vector<COMDLG_FILTERSPEC> fs;
            for (const auto& f : filters) {
                fs.push_back({ f.first.c_str(), f.second.c_str() });
            }
            hr = pFileOpen->SetFileTypes(fs.size(), fs.data());
            if (FAILED(hr)) {
                pFileOpen->Release();
                result->addErr("set filter err");
                return;
            }
        }

        hr = pFileOpen->Show(result->tar->hwnd);
        if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
            pFileOpen->Release();
            result->addBool("cancel", true);
            result->returnBackThread();
            return;
        }
        result->addBool("cancel", false);
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addErr("pFileOpen Show err");
            return;
        }
        bool multiSelection = (option & FOS_ALLOWMULTISELECT) == FOS_ALLOWMULTISELECT;
        if (multiSelection) {
            IShellItemArray* pResults = nullptr;
            hr = pFileOpen->GetResults(&pResults);
            if (FAILED(hr)) {
                pFileOpen->Release();
                result->addErr("get multi results err");
                return;
            }
            // 遍历选中的文件
            DWORD count = 0;
            hr = pResults->GetCount(&count);
            if(FAILED(hr)) {
                pResults->Release();
                pFileOpen->Release();
                result->addErr("get multi results count err");
                return;
            }
            rapidjson::Value array(rapidjson::kArrayType);
            for (DWORD i = 0; i < count; ++i) {
                IShellItem* pItem = nullptr;
                hr = pResults->GetItemAt(i, &pItem);
                if (FAILED(hr)) {
                    pResults->Release();
                    pFileOpen->Release();
                    result->addErr("get multi results count err");
                    return;
                }
                PWSTR pszFilePath = nullptr;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if (FAILED(hr)) {
                    pItem->Release();
                    pResults->Release();
                    pFileOpen->Release();
                    result->addErr("get multi results name err");
                    return;
                }
                auto str = Util::convertToStr(pszFilePath);
                rapidjson::Value val;
                val.SetString(str.data(), str.length(), result->getAllocator());
                array.PushBack(val, result->getAllocator());
                CoTaskMemFree(pszFilePath);
                pItem->Release();
            }
            pResults->Release();
            pFileOpen->Release(); 
            result->addValue("data", std::move(array));
        }
        else {
            IShellItem* pItem;
            hr = pFileOpen->GetResult(&pItem);
            if (FAILED(hr)) {
                pFileOpen->Release();
                result->addErr("pFileOpen GetResult err");
                return;
            }
            PWSTR pszFolderPath;
            hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
            if (FAILED(hr)) {
                CoTaskMemFree(pszFolderPath);
                pFileOpen->Release();
                result->addErr("GetDisplayName err");
                return;
            }
            std::wstring pathStr(pszFolderPath);
            CoTaskMemFree(pszFolderPath);
            pItem->Release();
            pFileOpen->Release();
            auto str = Util::convertToStr(pathStr);
            result->addString("data", str);
        }
        result->returnBackThread();
        
        });
    worker.detach();
          
}
