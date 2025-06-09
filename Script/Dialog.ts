import { Eventer } from "./Eventer";
export class Dialog extends Eventer {
  async openPathDialog(config: any) {
    return this.callMethod("openPathDialog", config);
  }
  private callMethod(methodName: string, ...params: any[]) {
    let obj = window.self === window.top ? this : window.top.horse.dialog;
    return obj.call({
      className: "dialog",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
