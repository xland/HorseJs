import { Eventer } from "./Eventer";
export class Dialog extends Eventer {
  async openPath(config: any) {
    return this.exec("openPath", config);
  }
  async savePath(config: any) {
    return this.exec("savePath", config);
  }
  async msgBox(config: any) {
    return this.exec("msgBox", config);
  }
  private exec(methodName: string, ...params: any[]) {
    return this.call({
      className: "dialog",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
