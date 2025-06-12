import { Eventer } from "./Eventer";
export class Lib extends Eventer {
  getDataType() {
    return this.exec("getDataType");
  }
  readText() {
    return this.exec("readText");
  }
  writeText(text: string) {
    return this.exec("writeText", text);
  }
  readHtml() {
    return this.exec("readHtml");
  }
  private exec(methodName: string, ...params: any[]) {
    return this.call({
      className: "lib",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
