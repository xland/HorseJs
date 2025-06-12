import { Eventer } from "./Eventer";
export class Notify extends Eventer {
  async show(appName: string, title: string, content: string) {
    return this.exec("show", appName, title, content);
  }
  private exec(methodName: string, ...params: any[]) {
    return this.call({
      className: "notify",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
