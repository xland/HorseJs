import { Eventer } from "./Eventer";
export class Db extends Eventer {
  async open(dbPath: string, inDataDir = true) {
    return this.exec("open", dbPath, inDataDir);
  }
  async close(dbPath: string) {
    return this.exec("close", dbPath);
  }
  async sql(sql: string, dbPath: string) {
    return this.exec("sql", sql, dbPath);
  }
  async del(dbPath: string, inDataDir = true) {
    return this.exec("del", dbPath, inDataDir);
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
