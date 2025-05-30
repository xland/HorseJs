import { Eventer } from "./Eventer";
export class Dialog extends Eventer {
  async openPathDialog(isOpenDir: boolean) {
    return this.callMethod("openPathDialog", isOpenDir);
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "dialog",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
