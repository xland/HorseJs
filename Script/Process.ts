import { Eventer } from "./Eventer";
export class Process extends Eventer {
  async start(path: string) {
    return this.exec("start", path);
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
