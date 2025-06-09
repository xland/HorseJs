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
  writeRtf(rtf: string) {
    return this.callMethod("writeRtf", rtf);
  }
  readImg() {
    return this.callMethod("readImg");
  }
  writeImg(rtf: string) {
    return this.callMethod("writeImg", rtf);
  }
  getFile() {
    return this.callMethod("getFile");
  }
  addFile(...paths) {
    return this.callMethod("addFile", ...paths);
  }
  clear() {
    return this.callMethod("clear");
  }
  private callMethod(methodName: string, ...params: any[]) {
    let obj = window.self === window.top ? this : window.top.horse.clipboard;
    return obj.call({
      className: "clipboard",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
