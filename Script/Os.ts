import { Eventer } from "./Eventer";
export class Os extends Eventer {
  async getVersion() {
    return this.exec("getVersion");
  }
  private exec(methodName: string, ...params: any[]) {
    return this.call({
      className: "os",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
