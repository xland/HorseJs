import { Eventer } from "./Eventer";
export class Db extends Eventer {
  async open(dbPath: string, inDataDir = true) {
    return this.execute("open", dbPath, inDataDir);
  }
  async close(dbPath: string) {
    return this.execute("close", dbPath);
  }
  async prepare(sql: string, dbPath: string) {
    return this.execute("prepare", sql, dbPath);
  }
  async exec(sql: string, dbPath: string) {
    return this.execute("exec", sql, dbPath);
  }
  async del(dbPath: string, inDataDir = true) {
    return this.execute("del", dbPath, inDataDir);
  }
  private execute(methodName: string, ...params: any[]) {
    return this.call({
      className: "db",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
