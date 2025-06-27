import { Eventer } from "./Eventer";
export class Dialog extends Eventer {
  openPath(config: any) {
    return this.execute("openPath", config);
  }
  savePath(config: any) {
    return this.execute("savePath", config);
  }
  msgBox(config: any) {
    return this.execute("msgBox", config);
  }
  itemInFolder(filePath: string) {
    return this.execute("itemInFolder", filePath);
  }
  private execute(methodName: string, ...params: any[]) {
    return this.call({
      className: "dialog",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
