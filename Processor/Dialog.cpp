#include <pch.h>

#include "../App/App.h"
#include "Dialog.h"
#include "../App/BrowserWindow.h"

namespace {
    std::unique_ptr<Dialog> dialog;
    static std::unordered_map<std::string, void (Dialog::*)(const rapidjson::Value&, JsonResult*)> funcs{
        {"openPath", &Dialog::openPath},
        {"savePath", &Dialog::savePath},
        {"msgBox", &Dialog::msgBox},
        {"itemInFolder", &Dialog::itemInFolder},
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
bool Dialog::execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Dialog::get()->*it->second)(param, result);
    return true;
}
void Dialog::openPath(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    const rapidjson::Value& obj = arr[0].GetObj();

    std::wstring title;
    if (obj.HasMember("title") && obj["title"].IsString()) {
        title = Util::convertToWStr(obj["title"].GetString());
    }

    std::wstring okBtnText;
    if (obj.HasMember("okBtnText") && obj["okBtnText"].IsString()) {
        okBtnText = Util::convertToWStr(obj["okBtnText"].GetString());
    }

    std::wstring defaultPath;
    if (obj.HasMember("defaultPath") && obj["defaultPath"].IsString()) {
        defaultPath = Util::convertToWStr(obj["defaultPath"].GetString());
    }

    FILEOPENDIALOGOPTIONS option{ FOS_FORCEFILESYSTEM };
    std::string type{"file"};
    if (obj.HasMember("type") && obj["type"].IsString()) {
        type = obj["type"].GetString();
        if (type == "dir") {
            option = FOS_PICKFOLDERS;
        }
    }
    filterType filters;
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
    result->cancel = true;

    auto asyncResult = new JsonResult(result->winId,"dialog",result->getString("eventName"));
    std::jthread worker(&Dialog::showOpenPathDialog,
        this,
        asyncResult,
        std::move(title),
        std::move(okBtnText),
        std::move(defaultPath),
        std::move(option),
        std::move(filters));
    worker.detach();
}

void Dialog::savePath(const rapidjson::Value& params, JsonResult* result)
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

    std::wstring defaultPath;
    if (obj.HasMember("defaultPath") && obj["defaultPath"].IsString()) {
        defaultPath = Util::convertToWStr(obj["defaultPath"].GetString());
    }

    FILEOPENDIALOGOPTIONS option{ FOS_FORCEFILESYSTEM };
    filterType filters;
    if (obj.HasMember("filter") && obj["filter"].IsArray()) {
        const rapidjson::Value::ConstArray arr = obj["filter"].GetArray();
        for (const auto& filter : arr) {
            const rapidjson::Value::ConstArray arr = filter.GetArray();
            auto name = Util::convertToWStr(arr[0].GetString());
            auto val = Util::convertToWStr(arr[1].GetString());
            filters.push_back({ name, val });
        }
    }

    bool showHiddenFile{ false };
    if (obj.HasMember("showHiddenFile") && obj["showHiddenFile"].IsBool()) {
        showHiddenFile = obj["showHiddenFile"].GetBool();
        if (showHiddenFile) {
            option |= FOS_FORCESHOWHIDDEN;
        }
    }
    result->cancel = true;

    auto asyncResult = new JsonResult(result->winId, "dialog", result->getString("eventName"));
    std::jthread worker(&Dialog::showSavePathDialog,
        this,
        asyncResult,
        std::move(title),
        std::move(okBtnText),
        std::move(defaultPath),
        std::move(option),
        std::move(filters));
    worker.detach();
}

void Dialog::msgBox(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    const rapidjson::Value& obj = arr[0].GetObj();
    std::wstring title;
    if (obj.HasMember("title") && obj["title"].IsString()) {
        title = Util::convertToWStr(obj["title"].GetString());
    }
    std::wstring msg;
    if (obj.HasMember("msg") && obj["msg"].IsString()) {
        msg = Util::convertToWStr(obj["msg"].GetString());
    }
    UINT icon = MB_ICONINFORMATION;  //info, warn, err, question,stop
    if (obj.HasMember("icon") && obj["icon"].IsString()) {
        std::string iconType = obj["icon"].GetString();
        if (iconType == "warn") {
            icon = MB_ICONWARNING;
        }
        else if (iconType == "err") {
            icon = MB_ICONERROR;
        }
        else if (iconType == "question") {
            icon = MB_ICONQUESTION;
        }
        else if (iconType == "stop") {
            icon = MB_ICONSTOP;
        }
    }
    UINT mbButton = MB_OK;  //"ok"
    if (obj.HasMember("btn") && obj["btn"].IsString()) {
        std::string btn = obj["btn"].GetString();
        if (btn == "okCancel") {
            mbButton = MB_OKCANCEL;
        }
        else if (btn == "yesNo") {
            mbButton = MB_YESNO;
        }
        else if (btn == "yesNoCancel") {
            mbButton = MB_YESNOCANCEL;
        }
        else if (btn == "retryCancel") {
            mbButton = MB_RETRYCANCEL;
        }
        else if (btn == "abortRetryIgnore") {
            mbButton = MB_ABORTRETRYIGNORE;
        }
    }

    result->cancel = true;
    auto iconAndBtns = mbButton | icon;
    auto asyncResult = new JsonResult(result->winId, "dialog", result->getString("eventName"));
    std::jthread worker([asyncResult,title = std::move(title),msg = std::move(msg),iconAndBtns ]() {
            int ret = MessageBox(App::getWindow(asyncResult->winId)->hwnd, msg.c_str(), title.c_str(), iconAndBtns);
            switch (ret) {
            case IDOK:
                asyncResult->addString("data", "ok");
                break;
            case IDCANCEL:
                asyncResult->addString("data", "cancel");
                break;
            case IDYES:
                asyncResult->addString("data", "yes");
                break;
            case IDNO:
                asyncResult->addString("data", "no");
                break;
            case IDABORT:
                asyncResult->addString("data", "abort");
                break;
            case IDRETRY:
                asyncResult->addString("data", "retry");
                break;
            case IDIGNORE:
                asyncResult->addString("data", "ignore");
                break;
            case 0:
                asyncResult->addErr("MessageBox failed: " + std::to_string(GetLastError()));
                return;
            default:
                asyncResult->addErr("unknown MessageBox return value: " + std::to_string(ret));
                return;
            }
            asyncResult->returnBackThread();
        });
    worker.detach();
}

void Dialog::showOpenPathDialog(JsonResult* result,
    const std::wstring&& title,
    const std::wstring&& okBtnText,
    const std::wstring&& defaultPath,
    const FILEOPENDIALOGOPTIONS&& option,
    const filterType&& filter)
{
    IFileOpenDialog* pFileOpen;
    auto hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));
    if (FAILED(hr)) {
        result->addErr("CoCreateInstance false");
        result->returnBackThread();
        return;
    }
    DWORD dwOptions;
    hr = pFileOpen->GetOptions(&dwOptions);
    if (FAILED(hr)) {
        pFileOpen->Release();
        result->addErr("pFileOpen GetOptions err");
        result->returnBackThread();
        return;
    }
    hr = pFileOpen->SetOptions(dwOptions | option);
    if (FAILED(hr)) {
        pFileOpen->Release();
        result->addErr("pFileOpen SetOptions err");
        result->returnBackThread();
        return;
    }
    if (!title.empty()) {
        hr = pFileOpen->SetTitle(title.data());
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addErr("set title err");
            result->returnBackThread();
            return;
        }
    }
    if (!okBtnText.empty()) {
        hr = pFileOpen->SetOkButtonLabel(okBtnText.data());
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addErr("set ok button lable err");
            result->returnBackThread();
            return;
        }
    }
    if (!defaultPath.empty()) {
        IShellItem* pDefaultFolder = nullptr;
        hr = SHCreateItemFromParsingName(defaultPath.c_str(), nullptr, IID_PPV_ARGS(&pDefaultFolder));
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addErr("set default dir err");
            result->returnBackThread();
            return;
        }
        hr = pFileOpen->SetDefaultFolder(pDefaultFolder);  //todo SetFolder
        pDefaultFolder->Release();
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addErr("set default dir err");
            result->returnBackThread();
            return;
        }
    }

    if (!filter.empty()) {
        std::vector<COMDLG_FILTERSPEC> fs;
        for (const auto& f : filter) {
            fs.push_back({ f.first.c_str(), f.second.c_str() });
        }
        hr = pFileOpen->SetFileTypes(fs.size(), fs.data());
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addErr("set filter err");
            result->returnBackThread();
            return;
        }
    }
    auto win = App::getWindow(result->winId);
    hr = pFileOpen->Show(win->hwnd);
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
        result->returnBackThread();
        return;
    }
    bool multiSelection = (option & FOS_ALLOWMULTISELECT) == FOS_ALLOWMULTISELECT;
    if (multiSelection) {
        IShellItemArray* pResults = nullptr;
        hr = pFileOpen->GetResults(&pResults);
        if (FAILED(hr)) {
            pFileOpen->Release();
            result->addErr("get multi results err");
            result->returnBackThread();
            return;
        }
        // 遍历选中的文件
        DWORD count = 0;
        hr = pResults->GetCount(&count);
        if (FAILED(hr)) {
            pResults->Release();
            pFileOpen->Release();
            result->addErr("get multi results count err");
            result->returnBackThread();
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
                result->returnBackThread();
                return;
            }
            PWSTR pszFilePath = nullptr;
            hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
            if (FAILED(hr)) {
                pItem->Release();
                pResults->Release();
                pFileOpen->Release();
                result->addErr("get multi results name err");
                result->returnBackThread();
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
            result->returnBackThread();
            return;
        }
        PWSTR pszFolderPath;
        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
        if (FAILED(hr)) {
            CoTaskMemFree(pszFolderPath);
            pFileOpen->Release();
            result->addErr("GetDisplayName err");
            result->returnBackThread();
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
}

void Dialog::showSavePathDialog(JsonResult* result,
    const std::wstring&& title,
    const std::wstring&& okBtnText,
    const std::wstring&& defaultPath,
    const FILEOPENDIALOGOPTIONS&& option,
    const filterType&& filter)
{
    // 创建 IFileSaveDialog 实例
    IFileSaveDialog* pFileSave;
    HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileSave));
    if (FAILED(hr)) {
        result->addErr("CoCreateInstance failed: " + std::to_string(hr));
        result->returnBackThread();
        return;
    }

    // 获取当前选项并移除 FOS_FILEMUSTEXIST
    DWORD dwOptions;
    hr = pFileSave->GetOptions(&dwOptions);
    if (FAILED(hr)) {
        pFileSave->Release();
        result->addErr("pFileSave GetOptions err: " + std::to_string(hr));
        result->returnBackThread();
        return;
    }

    // 设置选项，确保适合保存新文件
    dwOptions |= option;
    dwOptions &= ~FOS_FILEMUSTEXIST; // 移除 FOS_FILEMUSTEXIST
    dwOptions |= FOS_PATHMUSTEXIST | FOS_OVERWRITEPROMPT; // 确保路径存在，提示覆盖
    hr = pFileSave->SetOptions(dwOptions);
    if (FAILED(hr)) {
        pFileSave->Release();
        result->addErr("pFileSave SetOptions err: " + std::to_string(hr));
        result->returnBackThread();
        return;
    }

    // 设置标题
    if (!title.empty()) {
        hr = pFileSave->SetTitle(title.data());
        if (FAILED(hr)) {
            pFileSave->Release();
            result->addErr("set title err: " + std::to_string(hr));
            result->returnBackThread();
            return;
        }
    }

    // 设置确认按钮文本
    if (!okBtnText.empty()) {
        hr = pFileSave->SetOkButtonLabel(okBtnText.data());
        if (FAILED(hr)) {
            pFileSave->Release();
            result->addErr("set ok button label err: " + std::to_string(hr));
            result->returnBackThread();
            return;
        }
    }

    // 解析默认路径和文件名
    std::wstring dirPath;
    std::wstring fileName;
    if (!defaultPath.empty()) {
        // 查找最后一个路径分隔符
        size_t lastSlash = defaultPath.find_last_of(L"\\/");
        if (lastSlash != std::wstring::npos && lastSlash < defaultPath.length() - 1) {
            // 提取目录和文件名
            dirPath = defaultPath.substr(0, lastSlash);
            fileName = defaultPath.substr(lastSlash + 1);
        }
        else {
            // 仅目录，无文件名
            dirPath = defaultPath;
        }

        // 验证目录是否存在
        if (!dirPath.empty()) {
            DWORD dwAttrib = GetFileAttributesW(dirPath.c_str());
            if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
                pFileSave->Release();
                result->addErr("default directory does not exist and cannot be created: " + std::to_string(hr));
                result->returnBackThread();
                return;
            }

            // 设置默认目录
            IShellItem* pDefaultFolder = nullptr;
            hr = SHCreateItemFromParsingName(dirPath.c_str(), nullptr, IID_PPV_ARGS(&pDefaultFolder));
            if (FAILED(hr)) {
                pFileSave->Release();
                result->addErr("set default dir err: " + std::to_string(hr));
                result->returnBackThread();
                return;
            }
            hr = pFileSave->SetFolder(pDefaultFolder);
            pDefaultFolder->Release();
            if (FAILED(hr)) {
                pFileSave->Release();
                result->addErr("set default dir err: " + std::to_string(hr));
                result->returnBackThread();
                return;
            }
        }

        // 设置默认文件名（如果存在）
        if (!fileName.empty()) {
            hr = pFileSave->SetFileName(fileName.c_str());
            if (FAILED(hr)) {
                pFileSave->Release();
                result->addErr("set default file name err: " + std::to_string(hr));
                result->returnBackThread();
                return;
            }
        }
    }

    // 设置文件过滤器并指定默认扩展名
    std::wstring defaultExt;
    if (!filter.empty()) {
        std::vector<COMDLG_FILTERSPEC> fs;
        for (const auto& f : filter) {
            fs.push_back({ f.first.c_str(), f.second.c_str() });
        }
        hr = pFileSave->SetFileTypes(fs.size(), fs.data());
        if (FAILED(hr)) {
            pFileSave->Release();
            result->addErr("set filter err: " + std::to_string(hr));
            result->returnBackThread();
            return;
        }
        // 设置默认扩展名（使用第一个过滤器的扩展名）
        defaultExt = filter.begin()->second;
        if (defaultExt.find(L"*") != std::wstring::npos) {
            defaultExt.erase(0, 2); // 移除 "*. "
        }
        hr = pFileSave->SetDefaultExtension(defaultExt.c_str());
        if (FAILED(hr)) {
            pFileSave->Release();
            result->addErr("set default extension err: " + std::to_string(hr));
            result->returnBackThread();
            return;
        }
    }
    // 显示保存对话框
    auto win = App::getWindow(result->winId);
    hr = pFileSave->Show(win->hwnd);
    if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        pFileSave->Release();
        result->addBool("cancel", true);
        result->returnBackThread();
        return;
    }
    if (FAILED(hr)) {
        pFileSave->Release();
        result->addErr("pFileSave Show err: " + std::to_string(hr));
        result->returnBackThread();
        return;
    }
    result->addBool("cancel", false);

    // 获取保存的文件路径
    IShellItem* pItem;
    hr = pFileSave->GetResult(&pItem);
    if (FAILED(hr)) {
        pFileSave->Release();
        result->addErr("pFileSave GetResult err: " + std::to_string(hr));
        result->returnBackThread();
        return;
    }

    PWSTR pszFilePath;
    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
    if (FAILED(hr)) {
        pItem->Release();
        pFileSave->Release();
        result->addErr("GetDisplayName err: " + std::to_string(hr));
        result->returnBackThread();
        return;
    }
    // 验证文件名是否有效
    std::wstring pathStr(pszFilePath);
    // 转换路径并存储到结果
    CoTaskMemFree(pszFilePath);
    pItem->Release();
    pFileSave->Release();
    auto str = Util::convertToStr(pathStr);
    result->addString("data", str);
    result->returnBackThread();
}

void Dialog::itemInFolder(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto filePath = arr[0].GetString();
    auto str = Util::convertToWStr(filePath);
    std::wstring command = std::format(L"/select,\"{}\"", str);
    ShellExecute(nullptr, L"open", L"explorer.exe", command.c_str(), nullptr, SW_SHOW);
}