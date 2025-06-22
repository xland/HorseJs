#include <pch.h>
#include "Fs.h"
#include "../App/App.h"
#include "../App/BrowserWindow.h"

namespace {
    std::unique_ptr<Fs> fs;
    std::shared_mutex mtx;
    std::unordered_map<std::string, HANDLE> watchMap;
    static std::unordered_map<std::string, void (Fs::*)(const rapidjson::Value&, JsonResult*)> funcs{
    {"getFileInfo", &Fs::getFileInfo},
    {"exists", &Fs::exists},
    {"readFile", &Fs::readFile},
    {"readFileChunk",&Fs::readFileChunk},
    {"writeFile", &Fs::writeFile},
    {"writeFileChunk", &Fs::writeFileChunk},
    {"delPath", &Fs::delPath},
    {"removePath", &Fs::removePath},
    {"createFile", &Fs::createFile},
    {"createDir", &Fs::createDir},
    {"ensurePath", &Fs::ensurePath},
    {"listDir", &Fs::listDir},
    {"copyPath", &Fs::copyPath},
    {"movePath", &Fs::movePath},
    {"renamePath", &Fs::renamePath},
    {"getPath", &Fs::getPath},
    {"watch", &Fs::watch},
    {"stopWatch", &Fs::stopWatch},
    {"createShortcut", &Fs::createShortcut},
    {"openFile", &Fs::openFile},
    };
    //todo: 获取有几个逻辑磁盘
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
bool Fs::execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Fs::get()->*it->second)(param, result);
    return true;
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
    result->returnBackSharedBuffer(sharedBuffer.get());
    //关闭 C++ 端对共享缓冲区的访问权限。 通知操作系统，主进程不再持有该共享内存的引用。
    //此时共享内存由渲染进程持有，不会立即销毁。
    //渲染进程通过 chrome.webview.releaseBuffer 释放缓冲区。
    sharedBuffer->Close();
    result->cancel = true;
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
    result->returnBackSharedBuffer(sharedBuffer.get());
    //关闭 C++ 端对共享缓冲区的访问权限。 通知操作系统，主进程不再持有该共享内存的引用。
    //此时共享内存由渲染进程持有，不会立即销毁。
    //渲染进程通过 chrome.webview.releaseBuffer 释放缓冲区。
    sharedBuffer->Close();
    result->cancel = true;
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
}
void Fs::ensurePath(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring path = Util::convertToWStr(arr[0].GetString());    
    std::filesystem::path targetPath(path);
    if (std::filesystem::exists(targetPath)) {
        return;
    }
    std::vector<std::filesystem::path> directories;
    std::filesystem::path current = targetPath;
    auto hasExt = targetPath.has_extension();
    auto fileName = targetPath.filename().wstring();
    if (hasExt && !fileName.empty()) {
        current = targetPath.parent_path();
    }
    while (!current.empty() && !std::filesystem::exists(current)) {
        directories.push_back(current);
        current = current.parent_path();
    }
    for (auto it = directories.rbegin(); it != directories.rend(); ++it) {
        if (!CreateDirectory(it->wstring().c_str(), nullptr)) {
            DWORD error = GetLastError();
            if (error != ERROR_ALREADY_EXISTS) {
                result->addErr("create path exists");
                return;
            }
        }
    }
    if (hasExt && !fileName.empty()) {
        std::wofstream file(targetPath.wstring(), std::ios::out);
        if (!file.is_open()) {
            result->addErr("file create err");
            return;
        }
        file.close();
    }
}
void Fs::createFile(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring path = Util::convertToWStr(arr[0].GetString());
    std::wofstream file(path, std::ios::out);
    if (!file.is_open()) {
        result->addErr("file create err");
        return;
    }
    file.close();
}
void Fs::createDir(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring path = Util::convertToWStr(arr[0].GetString());
    std::filesystem::path dirPath(path);
    if (std::filesystem::exists(dirPath)) {
        result->addErr("path exists");
        return;
    }
    BOOL flag = CreateDirectory(path.c_str(), nullptr);
    if (!flag) {
        result->addErr("create path err");
        return;
    }
}
void Fs::listDir(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring startDirectory = Util::convertToWStr(arr[0].GetString());
    rapidjson::Value array(rapidjson::kArrayType);
    std::stack<std::wstring> dirStack;
    dirStack.push(startDirectory);
    auto& allocator = result->getAllocator();
    while (!dirStack.empty()) {
        std::wstring currentDir = dirStack.top();
        dirStack.pop();
        std::wstring searchPath = currentDir + L"\\*.*";
        WIN32_FIND_DATAW findFileData;
        HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findFileData);
        if (hFind == INVALID_HANDLE_VALUE) {
            result->addErr("opening directory err");
            continue;
        }
        do {
            // 跳过 . 和 .. 目录
            if (wcscmp(findFileData.cFileName, L".") == 0 || wcscmp(findFileData.cFileName, L"..") == 0) 
            {
                continue;
            }
            std::wstring fullPath = currentDir + L"\\" + findFileData.cFileName;
            rapidjson::Value obj(rapidjson::kObjectType);            
            rapidjson::Value key1("path", allocator);
            rapidjson::Value key2("isFile", allocator);
            auto valStr = Util::convertToStr(fullPath);
            rapidjson::Value val(valStr.data(), allocator);
            obj.AddMember(key1, val, allocator);
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                obj.AddMember(key2, false, allocator);
                // 将子目录压入栈中
                dirStack.push(fullPath);
            }
            else {
                obj.AddMember(key2, true, allocator);
            }
            array.PushBack(obj, allocator);
        } while (FindNextFileW(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
    result->addValue("data", std::move(array));
}
void Fs::copyPath(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring src = Util::convertToWStr(arr[0].GetString());
    std::wstring dst = Util::convertToWStr(arr[1].GetString());
    const wchar_t* srcData = src.data();
    if (GetFileAttributes(src.c_str()) == INVALID_FILE_ATTRIBUTES) {
        result->addErr("src path not exists");
        return;
    }
    src.push_back(L'\0');
    dst.push_back(L'\0');
    SHFILEOPSTRUCTW fileOp = { 0 };
    fileOp.wFunc = FO_COPY;
    fileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
    fileOp.pFrom = src.data();
    fileOp.pTo = dst.data();
    int ret = SHFileOperation(&fileOp);
    if (ret != 0) {
        result->addErr("move err");
    }
}

void Fs::movePath(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring src = Util::convertToWStr(arr[0].GetString());
    std::wstring dst = Util::convertToWStr(arr[1].GetString());
    const wchar_t* srcData = src.data();
    if (GetFileAttributes(src.c_str()) == INVALID_FILE_ATTRIBUTES) {
        result->addErr("src path not exists");
        return;
    }
    src.push_back(L'\0');
    dst.push_back(L'\0');
    SHFILEOPSTRUCTW fileOp = { 0 };
    fileOp.wFunc = FO_MOVE;
    fileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
    fileOp.pFrom = src.data();
    fileOp.pTo = dst.data();
    int ret = SHFileOperation(&fileOp);
    if (ret != 0) {
        result->addErr("move err");
    }
}

void Fs::renamePath(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring src = Util::convertToWStr(arr[0].GetString());
    std::wstring dst = Util::convertToWStr(arr[1].GetString());
    const wchar_t* srcData = src.data();
    if (GetFileAttributes(src.c_str()) == INVALID_FILE_ATTRIBUTES) {
        result->addErr("src path not exists");
        return;
    }
    src.push_back(L'\0');
    dst.push_back(L'\0');
    SHFILEOPSTRUCT fileOp = { 0 };
    fileOp.hwnd = NULL;
    fileOp.wFunc = FO_RENAME;
    fileOp.pFrom = src.c_str();
    fileOp.pTo = dst.c_str();
    fileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    int val = SHFileOperation(&fileOp);
    if (val != 0 || fileOp.fAnyOperationsAborted != FALSE) {
        result->addErr("remove path error.");
    }
}
void Fs::getPath(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    if (arr.Size() < 1 || arr[0].IsNull() || !arr[0].IsString()) {
        result->addErr("type err");
        return;
    }
    std::string type{ arr[0].GetString() };
    if (type == "exeDir" || type == "exePath") {
        result->addString("data", getExePath(type));
    }
    else if (type == "download") {
        getKnownPath(FOLDERID_Downloads, result);
    }
    else if (type == "music") {
        getKnownPath(FOLDERID_Music, result);
    }
    else if (type == "video") {
        getKnownPath(FOLDERID_Videos, result);
    }
    else if (type == "picture") {
        getKnownPath(FOLDERID_Pictures, result);
    }
    else if (type == "document") {
        getKnownPath(FOLDERID_Documents, result);
    }
    else if (type == "startup") {
        getKnownPath(FOLDERID_Startup, result);
    }
    else if (type == "desktop") {
        getKnownPath(FOLDERID_Desktop, result);
    }
    else if (type == "font") {
        getKnownPath(FOLDERID_Fonts, result);
    }
    else if (type == "program") {
        getKnownPath(FOLDERID_ProgramFiles, result);
    }
    else if (type == "system") {
        getKnownPath(FOLDERID_System, result);
    }
    else if (type == "windows") {
        getKnownPath(FOLDERID_Windows, result);
    }
    else if (type == "profile") {
        getKnownPath(FOLDERID_Profile, result);
    }
    else if (type == "appdata") {
        getKnownPath(FOLDERID_LocalAppData, result);
    }
    else if (type == "roaming") {
        getKnownPath(FOLDERID_RoamingAppData, result);
    }
    else if (type == "cache") {
        getKnownPath(FOLDERID_InternetCache, result);
    }
    else if (type == "userdata") {
        auto str = App::get()->appDir.string();
        result->addString("data", str);
    }
}
void Fs::watch(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring src = Util::convertToWStr(arr[0].GetString());
    std::string id = arr[1].GetString();
    result->addString("id", id);
    // 打开目录句柄，以便对其进行文件系统更改通知的监控
    HANDLE hDir = CreateFile(src.data(), FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
    if (hDir == INVALID_HANDLE_VALUE) {
        result->addErr("create dir handle error.");
        return;
    }
    Fs::addWatch(id, hDir);
    std::jthread worker([winId = result->winId, hDir,id=std::move(id)]() {
        BYTE buffer[1024];// 缓冲区用于存储变化信息
        DWORD bytesReturned;
        FILE_NOTIFY_INFORMATION* pNotify;
        auto lastTime = std::chrono::steady_clock::now();
        const auto debounceTime = std::chrono::milliseconds(80); // 合并时间窗口        
        while (App::getWindow(winId) && Fs::hasWatch(id)) {
            auto flags = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE;
            if (ReadDirectoryChangesW(hDir, buffer, sizeof(buffer),TRUE,/*子目录*/ flags, &bytesReturned, NULL, NULL) && bytesReturned > 0)
            {
                pNotify = (FILE_NOTIFY_INFORMATION*)buffer;
                do {
                    auto curTime = std::chrono::steady_clock::now(); // 当前时间
                    if ((curTime - lastTime) < debounceTime) {
                        if (pNotify->NextEntryOffset == 0) break;
                        pNotify = (FILE_NOTIFY_INFORMATION*)((BYTE*)pNotify + pNotify->NextEntryOffset);
                        continue;
                    }
                    lastTime = curTime;
                    auto result = new JsonResult(winId, "fs", id);
                    std::wstring fileName(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
                    auto fileNameStr = Util::convertToStr(fileName);
                    result->addString("file", fileNameStr.data());
                    switch (pNotify->Action) {
                    case FILE_ACTION_ADDED:
                        result->addString("type", "add");
                        break;
                    case FILE_ACTION_REMOVED:
                        result->addString("type", "remove");
                        break;
                    case FILE_ACTION_MODIFIED:
                        result->addString("type", "modify");
                        break;
                    case FILE_ACTION_RENAMED_OLD_NAME:
                        result->addString("type", "renaming");
                        break;
                    case FILE_ACTION_RENAMED_NEW_NAME:
                        result->addString("type", "renamed");
                        break;
                    default:
                        result->addString("type", "undefined");
                        break;
                    }
                    result->returnBackThread();
                    if (pNotify->NextEntryOffset == 0) break;
                    pNotify = (FILE_NOTIFY_INFORMATION*)((BYTE*)pNotify + pNotify->NextEntryOffset); // 移动到下一个通知
                } while (true);
            }
            else {
                break;
            }
        }
        });
    worker.detach();
}
void Fs::stopWatch(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::string id = arr[0].GetString();
    CloseHandle(watchMap[id]);
    Fs::removeWatch(id);
}
void Fs::packHorse(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring src = Util::convertToWStr(arr[0].GetString());
    std::filesystem::path targetPath(src);
	auto pPath = targetPath.parent_path();


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

void Fs::addWatch(const std::string& id, HANDLE handle)
{
    std::unique_lock<std::shared_mutex> lock(mtx);
    watchMap.insert({ id ,handle});
}

bool Fs::hasWatch(const std::string& id)
{
    std::shared_lock<std::shared_mutex> lock(mtx);
    auto flag = watchMap.contains(id);
    return flag;
}

void Fs::removeWatch(const std::string& id)
{
    std::unique_lock<std::shared_mutex> lock(mtx);
    watchMap.erase(id);
}
std::string Fs::getExePath(const std::string& type)
{
    wchar_t buffer[MAX_PATH];
    GetModuleFileName(nullptr, buffer, MAX_PATH);
    auto curPath = std::filesystem::path(buffer);
    if (type == "exeDir") {
        curPath = curPath.parent_path();
    }
    auto curPathStr = curPath.string();
    return curPathStr;
}

void Fs::getKnownPath(const GUID& type, JsonResult* result)
{
    PWSTR path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(type, 0, nullptr, &path);
    if (SUCCEEDED(hr)) {
        std::wstring strW(path);
        auto str = Util::convertToStr(strW);
        CoTaskMemFree(path);
        result->addString("data", str);
    }
    else {
        result->addErr("get known folder err");
    }
}

void Fs::enumFiles(const std::wstring& baseDir, const std::wstring& currentDir, std::vector<std::wstring>& fileList)
{
    std::wstring searchPath = currentDir + L"\\*";
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &findData);
    if (hFind == INVALID_HANDLE_VALUE) return;

    do {
        const std::wstring name = findData.cFileName;
        if (name == L"." || name == L"..") continue;

        std::wstring fullPath = currentDir + L"\\" + name;
        std::wstring relativePath = fullPath.substr(baseDir.length() + 1);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            enumFiles(baseDir, fullPath, fileList);
        }
        else {
            fileList.emplace_back(relativePath);
        }
    } while (FindNextFile(hFind, &findData));
    FindClose(hFind);


    //std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    //std::streamsize size = file.tellg();
    //file.seekg(0, std::ios::beg);

    //std::vector<char> buffer(size);
    //file.read(buffer.data(), size);
}
void Fs::createShortcut(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto srcPath = arr[0].GetString();
    auto srcPathW = Util::convertToWStr(srcPath);
    auto dstPath = arr[1].GetString();
    auto dstPathW = Util::convertToWStr(dstPath);
    auto des = arr[2].GetString();
    auto desW = Util::convertToWStr(des);
    auto workDir = arr[3].GetString();
    auto workDirW = Util::convertToWStr(workDir);

    IShellLink* pShellLink = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pShellLink);
    if (FAILED(hr)) {
        result->addErr("Failed to create IShellLink instance.");
        return;
    }
    pShellLink->SetPath(srcPathW.c_str());
    pShellLink->SetDescription(desW.c_str());
    pShellLink->SetIconLocation(srcPathW.c_str(), 0);
    pShellLink->SetWorkingDirectory(workDirW.c_str());
    IPersistFile* pPersistFile = nullptr;
    hr = pShellLink->QueryInterface(IID_IPersistFile, (LPVOID*)&pPersistFile);
    if (FAILED(hr)) {
        pShellLink->Release();
        result->addErr("Failed to query IPersistFile interface.");
        return;
    }
    hr = pPersistFile->Save(dstPathW.c_str(), TRUE);
    if (FAILED(hr)) {
        result->addErr("Failed to save shortcut file.");
    }
    pPersistFile->Release();
    pShellLink->Release();
}

void Fs::openFile(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto filePath = arr[0].GetString();
    auto str = Util::convertToWStr(filePath);
    HINSTANCE hr = ShellExecuteW(nullptr, L"open", str.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
	if (reinterpret_cast<intptr_t>(hr) <= 32) 
    {
		result->addErr("Failed to open file");
    }
}