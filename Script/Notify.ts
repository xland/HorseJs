import { Eventer } from "./Eventer";
export class Notify extends Eventer {
  async show(appName: string, title: string, content: string) {
    return this.callMethod("show", appName, title, content);
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "notify",
      winId: globalThis.__WIN_ID,
      tarId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
