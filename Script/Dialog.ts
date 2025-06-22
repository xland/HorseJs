import { Eventer } from "./Eventer";
export class Dialog extends Eventer {
  openPath(config: any) {
    return this.exec("openPath", config);
  }
  savePath(config: any) {
    return this.exec("savePath", config);
  }
  msgBox(config: any) {
    return this.exec("msgBox", config);
  }
  itemInFolder(filePath: string) {
    return this.exec("itemInFolder", filePath);
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
