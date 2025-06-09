import { Eventer } from "./Eventer";
export class Lib extends Eventer {
  getDataType() {
    return this.callMethod("getDataType");
  }
  readText() {
    return this.callMethod("readText");
  }
  writeText(text: string) {
    return this.callMethod("writeText", text);
  }
  readHtml() {
    return this.callMethod("readHtml");
  }
  private callMethod(methodName: string, ...params: any[]) {
    let obj = window.self === window.top ? this : window.top.horse.lib;
    return obj.call({
      className: "lib",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
