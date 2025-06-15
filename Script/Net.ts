import { Eventer } from "./Eventer";
export class Net extends Eventer {
  getAddress() {
    return this.exec("getAddress");
  }
  on(eventName, func) {
    let flag = this.listen(eventName, func);
    if (flag) {
      return this.exec("on", eventName);
    }
    return { "ok": true };
  }
  off(eventName, func) {
    let flag = this.unlisten(eventName, func);
    if (flag) {
      return this.exec("off", eventName);
    }
    return { "ok": true };
  }
  private exec(methodName: string, ...params: any[]) {
    return this.call({
      className: "net",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
