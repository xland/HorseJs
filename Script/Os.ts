import { Eventer } from "./Eventer";
export class Os extends Eventer {
  async getVersion() {
    return this.callMethod("getVersion");
  }
  private callMethod(methodName: string, ...params: any[]) {
    let obj = window.self === window.top ? this : window.top.horse.os;
    return obj.call({
      className: "os",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
