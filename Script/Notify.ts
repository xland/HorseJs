import { Eventer } from "./Eventer";
export class Notify extends Eventer {
  async show(appName: string, title: string, content: string) {
    return this.callMethod("show", appName, title, content);
  }
  private callMethod(methodName: string, ...params: any[]) {
    let obj = window.self === window.top ? this : window.top.notify;
    return obj.call({
      className: "notify",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
