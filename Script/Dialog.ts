import { Eventer } from "./Eventer";
export class Dialog extends Eventer {
  async openPath(config: any) {
    return this.callMethod("openPath", config);
  }
  async savePath(config: any) {
    return this.callMethod("savePath", config);
  }
  async msgBox(config: any) {
    return this.callMethod("msgBox", config);
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
