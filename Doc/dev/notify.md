void Notify::createShortcutWithAUMID(const std::wstring& aumid)
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileName(nullptr, buffer, MAX_PATH);
    std::wstring exePath{ buffer };
    auto curPath = std::filesystem::path(buffer).parent_path();
    auto curPathStr = curPath.wstring();
    std::wstring shortcutPath = L"C:\\Users\\liuxiaolun\\AppData\\Roaming\\HorseJs\\app.lnk";
    IShellLink* pShellLink = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pShellLink);
    if (SUCCEEDED(hr)) {
        pShellLink->SetPath(exePath.c_str());
        pShellLink->SetDescription(L"HorseJs");
        pShellLink->SetIconLocation(exePath.c_str(), 0);
        pShellLink->SetWorkingDirectory(curPathStr.c_str());
        IPropertyStore* pPropStore = nullptr;
        hr = pShellLink->QueryInterface(IID_IPropertyStore, (LPVOID*)&pPropStore);
        if (SUCCEEDED(hr)) {
            PROPVARIANT pv;
            InitPropVariantFromString(aumid.c_str(), &pv);
            pPropStore->SetValue(PKEY_AppUserModel_ID, pv);
            PropVariantClear(&pv);
            pPropStore->Commit();
            pPropStore->Release();
        }
        IPersistFile* pPersistFile = nullptr;
        hr = pShellLink->QueryInterface(IID_IPersistFile, (LPVOID*)&pPersistFile);
        if (SUCCEEDED(hr)) {
            pPersistFile->Save(shortcutPath.c_str(), TRUE);
            pPersistFile->Release();
        }
        pShellLink->Release();
    }
}

std::wstring Notify::getIcon() {
    auto logoPath = App::get()->appDir / "logo.ico";
    auto logoStr = logoPath.wstring();
    if (std::filesystem::exists(logoPath)) {
        return logoStr;
    }

    // 获取模块句柄
    HMODULE hModule = GetModuleHandle(nullptr);

    // 查找图标资源
    HRSRC hResource = FindResourceW(hModule, MAKEINTRESOURCE(IDI_LOGO), RT_ICON);
    if (!hResource) {
        std::wcerr << L"Failed to find icon resource" << std::endl;
        return L"";
    }

    // 加载资源
    HGLOBAL hResourceData = LoadResource(hModule, hResource);
    if (!hResourceData) {
        std::wcerr << L"Failed to load icon resource" << std::endl;
        return L"";
    }

    // 锁定资源，获取数据指针
    LPVOID pResourceData = LockResource(hResourceData);
    if (!pResourceData) {
        std::wcerr << L"Failed to lock icon resource" << std::endl;
        return L"";
    }

    // 获取资源大小
    DWORD resourceSize = SizeofResource(hModule, hResource);
    if (resourceSize == 0) {
        std::wcerr << L"Invalid resource size" << std::endl;
        return L"";
    }

    // 创建 ICO 文件
    HANDLE hFile = CreateFileW(logoStr.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Failed to create ICO file: " << logoStr << std::endl;
        return L"";
    }

    // 写入 ICO 数据
    DWORD bytesWritten;
    if (!WriteFile(hFile, pResourceData, resourceSize, &bytesWritten, nullptr) || bytesWritten != resourceSize) {
        CloseHandle(hFile);
        std::wcerr << L"Failed to write ICO file" << std::endl;
        return L"";
    }

    // 关闭文件
    CloseHandle(hFile);
    return logoStr;
}