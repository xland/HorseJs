import { Eventer } from "./Eventer";
export class Clipboard extends Eventer {
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
  writeHtml(html: string) {
    return this.callMethod("writeHtml", html);
  }
  readRtf() {
    return this.callMethod("readRtf");
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "clipboard",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
