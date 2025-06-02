import { Eventer } from "./Eventer";
export class Net extends Eventer {
  async getAddress() {
    return this.callMethod("getAddress");
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "net",
      winId: globalThis.__WIN_ID,
      tarId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
