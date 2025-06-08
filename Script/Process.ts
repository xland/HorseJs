import { Eventer } from "./Eventer";
export class Process extends Eventer {
  async exec(path: string) {
    return this.callMethod("exec", path);
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "process",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
