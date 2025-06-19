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
  private exec(methodName: string, ...params: any[]) {
    return this.call({
      className: "os",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
