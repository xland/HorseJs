#include "pch.h"
#include <sqlite/sqlite3.h>
#include "Db.h"


namespace {
    std::unique_ptr<Db> db;
    std::map<int, sqlite3*> dbMap;
    static std::unordered_map<std::string, void (Db::*)(const rapidjson::Value&, JsonResult*)> funcs{
        {"open", &Db::open},
        {"sql", &Db::sql},
        {"close", &Db::close},
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
    std::string dbPath = arr[0].GetString();
    sqlite3* dbIns;
    int rc = sqlite3_open(dbPath.data(), &dbIns);
    sqlite3_exec(dbIns, "PRAGMA journal_mode=WAL;", 0, 0, 0);
    if (rc) {
        std::string errInfo = std::format("打开数据库失败: {}", sqlite3_errmsg(dbIns));
        result->addErr(errInfo);
    }
    dbMap.insert({0,dbIns});
}

void Db::sql(const rapidjson::Value& params, JsonResult* result)
{
    const rapidjson::Value::ConstArray arr = params.GetArray();
    std::string sql = arr[0].GetString();
    auto dbId = arr[1].GetInt();
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(dbMap[dbId], sql.data(), -1, &stmt, nullptr);

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
    auto dbId = arr[0].GetInt();
    sqlite3_close(dbMap[dbId]);
    dbMap.erase(dbId);
}
