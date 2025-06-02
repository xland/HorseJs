import { Eventer } from "./Eventer";
export class Dialog extends Eventer {
  async openPathDialog(config: any) {
    return this.callMethod("openPathDialog", config);
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "dialog",
      winId: globalThis.__WIN_ID,
      tarId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
