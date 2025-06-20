import { Eventer } from "./Eventer";
export class Os extends Eventer {
  getVersion() {
    return this.exec("getVersion");
  }
  createShortcut(srcPath: string, dstSrc: string, des: string, workDir: string) {
    return this.exec("createShortcut", srcPath, dstSrc, des, workDir);
  }
  getCPUID() {
    return this.exec("getCPUID");
  }
  getDiskSerialNumber() {
    return this.exec("getDiskSerialNumber");
  }
  getUserLang() {
    return this.exec("getUserLang");
  }
  getOsLang() {
    return this.exec("getOsLang");
  }
  getOsColor() {
    return this.exec("getOsColor");
  }
  showItemInFolder(filePath: string) {
    return this.exec("showItemInFolder", filePath);
  }
  openFile(filePath: string) {
    return this.exec("openFile", filePath);
  }
  preventSleep() {
    return this.exec("preventSleep");
  }
  stopPreventSleep() {
    return this.exec("stopPreventSleep");
  }
  on(eventName, func) {
    let flag = this.listen(eventName, func);
    if (flag) {
      this.exec("on", eventName);
    }
  }
  off(eventName, func) {
    let flag = this.unlisten(eventName, func);
    if (flag) {
      this.exec("off", eventName);
    }
  }
  private exec(methodName: string, ...params: any[]) {
    return this.call({
      className: "os",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
