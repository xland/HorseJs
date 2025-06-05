import { Eventer } from "./Eventer";
export class Os extends Eventer {
  async getVersion() {
    return this.callMethod("getVersion");
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "os",
      winId: globalThis.__WIN_ID,
      tarId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
