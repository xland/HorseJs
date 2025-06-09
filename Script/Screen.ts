import { Eventer } from "./Eventer";
export class Screen extends Eventer {
  async getAll() {
    return this.callMethod("getAll");
  }
  private callMethod(methodName: string, ...params: any[]) {
    let obj = window.self === window.top ? this : window.top.horse.screen;
    return obj.call({
      className: "screen",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
