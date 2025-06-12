import { Eventer } from "./Eventer";
export class Clipboard extends Eventer {
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
  writeHtml(html: string) {
    return this.exec("writeHtml", html);
  }
  readRtf() {
    return this.exec("readRtf");
  }
  writeRtf(rtf: string) {
    return this.exec("writeRtf", rtf);
  }
  readImg() {
    return this.exec("readImg");
  }
  writeImg(rtf: string) {
    return this.exec("writeImg", rtf);
  }
  getFile() {
    return this.exec("getFile");
  }
  addFile(...paths) {
    return this.exec("addFile", ...paths);
  }
  clear() {
    return this.exec("clear");
  }
  private exec(methodName: string, ...params: any[]) {
    return this.call({
      className: "clipboard",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
