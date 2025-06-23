import { Eventer } from "./Eventer";
export class Db extends Eventer {
  async open(dbPath: string, inDbDir = true) {
    return this.exec("open", dbPath, inDbDir);
  }
  async close(dbPath: string) {
    return this.exec("close", dbPath);
  }
  async sql(sql: string, dbPath: string) {
    return this.exec("sql", sql, dbPath);
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
