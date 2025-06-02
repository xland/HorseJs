import { Eventer } from "./Eventer";
export class Clipboard extends Eventer {
  async readText() {
    return this.callMethod("readText");
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "clipboard",
      winId: globalThis.__WIN_ID,
      tarId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
