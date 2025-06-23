#include "pch.h"
#include <sqlite/sqlite3.h>
#include "Db.h"
#include "../App/App.h"


namespace {
    std::unique_ptr<Db> db;
    std::unordered_map<std::wstring, sqlite3*> dbMap;
    static std::unordered_map<std::string, void (Db::*)(const rapidjson::Value&, JsonResult*)> funcs{
        {"open", &Db::open},
        {"sql", &Db::sql},
        {"close", &Db::close},
        {"del", &Db::del},
    };
}

Db::Db()
{
}

Db::~Db()
{
}

Db* Db::get()
{
    if (!db) {
        db = std::make_unique<Db>();
    }
    return db.get();
}

bool Db::execute(std::string& methodName, const rapidjson::Value& param, JsonResult* result)
{
    auto it = funcs.find(methodName);
    if (it == funcs.end()) return false;
    (Db::get()->*it->second)(param, result);
    return true;
}

void Db::open(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring dbName = Util::convertToWStr(arr[0].GetString());
    bool inDataDir = arr[1].GetBool();
    sqlite3* dbIns;
    int rc;
    if (inDataDir) {
        auto dbPath = App::get()->appDir / dbName;
        auto dbPathStr = dbPath.string();
        rc = sqlite3_open(dbPathStr.data(), &dbIns);
    }
    else {
        std::filesystem::path dbPath(dbName);
        if (!dbPath.is_absolute()) {
            dbPath = std::filesystem::absolute(dbPath);
            dbName = dbPath.wstring();
        }
        auto dbPathStr = dbPath.string();
        rc = sqlite3_open(dbPathStr.data(), &dbIns);
    }
    sqlite3_exec(dbIns, "PRAGMA journal_mode=WAL;", 0, 0, 0);
    if (rc) {
        std::string errInfo = std::format("open db err: {}", sqlite3_errmsg(dbIns));
        result->addErr(errInfo);
    }
    dbMap.insert({ dbName,dbIns });
    auto str = Util::convertWstringToUtf8(dbName);
    result->addString("data", str);
}

void Db::sql(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::string sql = arr[0].GetString();
    auto dbName = Util::convertToWStr(arr[1].GetString());
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(dbMap[dbName], sql.data(), -1, &stmt, nullptr);

    rapidjson::Value array(rapidjson::kArrayType);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        JsonParsor dataItem;
        int colCount = sqlite3_column_count(stmt);
        for (int i = 0; i < colCount; i++) {
            std::string colName = sqlite3_column_name(stmt, i);
            int colType = sqlite3_column_type(stmt, i);
            switch (colType) {
                case SQLITE_INTEGER:{
                    dataItem.addNumber(colName, sqlite3_column_int64(stmt, i));
                    break;
                }
                case SQLITE_FLOAT: {
                    dataItem.addNumber(colName, sqlite3_column_double(stmt, i));
                    break;
                }
                case SQLITE_TEXT: {
                    std::string str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                    dataItem.addString(colName, str);
                    break;
                }
                case SQLITE_NULL: {
                    auto val = rapidjson::Value();
                    val.SetNull();
                    dataItem.addValue(colName, std::move(val));
                    break;
                }
            }
        }
        array.PushBack(dataItem.getVal(), result->getAllocator());
    }
    sqlite3_finalize(stmt);
    result->addValue("data", std::move(array));
}

void Db::close(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    auto dbName = Util::convertToWStr(arr[0].GetString());
    sqlite3_close(dbMap[dbName]);
    dbMap.erase(dbName);
}

void Db::del(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::wstring dbName = Util::convertToWStr(arr[0].GetString());
    if (dbMap.contains(dbName)) {
        sqlite3_close(dbMap[dbName]);
        dbMap.erase(dbName);
    }
    bool inDbDir = arr[1].GetBool();
    std::filesystem::path dbPath;
    if (inDbDir) {
        dbPath = App::get()->appDir / dbName;
    }
    else {
        dbPath = std::filesystem::path(dbName);
    }
    try {
        if (std::filesystem::exists(dbPath)) {
            bool removed = std::filesystem::remove(dbPath);
            if (!removed) {
                result->addErr("can not remove db");
                return;
            }
        }
        else {
            result->addErr("file not exists");
            return;
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        result->addErr("remove db err");
        return;
    }
}
