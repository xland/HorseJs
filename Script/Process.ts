import { Eventer } from "./Eventer";
export class Process extends Eventer {
  async exec(path: string) {
    return this.exec("exec", path);
  }
  private exec(methodName: string, ...params: any[]) {
    return this.call({
      className: "process",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
