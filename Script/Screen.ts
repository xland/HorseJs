import { Eventer } from "./Eventer";
export class Screen extends Eventer {
  async getAll() {
    return this.callMethod("getAll");
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "screen",
      winId: globalThis.__WIN_ID,
      tarId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
