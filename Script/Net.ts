import { Eventer } from "./Eventer";
export class Net extends Eventer {
  async getAddress() {
    return this.callMethod("getAddress");
  }
  private callMethod(methodName: string, ...params: any[]) {
    let obj = window.self === window.top ? this : window.top.horse.net;
    return obj.call({
      className: "net",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
