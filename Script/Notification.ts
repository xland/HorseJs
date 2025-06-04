import { Eventer } from "./Eventer";
export class Notification extends Eventer {
  async show() {
    return this.callMethod("show");
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "notification",
      winId: globalThis.__WIN_ID,
      tarId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
