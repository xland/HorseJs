import { Eventer } from "./Eventer";
export class Clipboard extends Eventer {
  getDataType() {
    return this.execute("getDataType");
  }
  readText() {
    return this.execute("readText");
  }
  writeText(text: string) {
    return this.execute("writeText", text);
  }
  readHtml() {
    return this.execute("readHtml");
  }
  writeHtml(html: string) {
    return this.execute("writeHtml", html);
  }
  readRtf() {
    return this.execute("readRtf");
  }
  writeRtf(rtf: string) {
    return this.execute("writeRtf", rtf);
  }
  readImg() {
    return this.execute("readImg");
  }
  writeImg(rtf: string) {
    return this.execute("writeImg", rtf);
  }
  getFile() {
    return this.execute("getFile");
  }
  addFile(...paths) {
    return this.execute("addFile", ...paths);
  }
  clear() {
    return this.execute("clear");
  }
  private execute(methodName: string, ...params: any[]) {
    return this.call({
      className: "clipboard",
      winId: horse.win.id,
      methodName,
      params,
    });
  }
}
