import { Eventer } from "./Eventer";
export class Tray extends Eventer {
  async create() {
    return this.callMethod("create");
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "create",
      winId: globalThis.__WIN_ID,
      tarId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
