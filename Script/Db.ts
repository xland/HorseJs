import { Eventer } from "./Eventer";
export class Db extends Eventer {
  async open(dbPath: string, inDbDir = true) {
    return this.exec("open", dbPath, inDbDir);
  }
  async close(id = 0) {
    return this.exec("close", id);
  }
  async sql(sql: string, id = 0) {
    return this.exec("sql", sql, id);
  }
  async del(dbPath: string, inDbDir = true) {
    return this.exec("del", dbPath, inDbDir);
  }
  private exec(methodName: string, ...params: any[]) {
    return this.call({
      className: "db",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
