import { Eventer } from "./Eventer";
import { util } from "./Util";
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
  getIpAddr() {
    return this.exec("getIpAddr");
  }
  showNotify(appName: string, title: string, content: string) {
    return this.exec("showNotify", appName, title, content);
  }
  createTray(config: any) {
    config.__id = util.randomNum();
    this.listen(config.__id, (data) => {
      let type = data.type;
      delete data.type;
      config[type]();
    });
    config.menu.forEach((item) => {
      item.__id = util.randomNum();
      this.listen(item.__id, item.click);
    });
    return this.exec("createTray", config);
  }
  destroyTray(id: number) {
    return this.exec("destroyTray", id);
  }
  spawn(path: string) {
    return this.exec("spawn", path);
  }
  creadWrite(key: string, val: string) {
    return this.exec("credWrite", key, val);
  }
  credRead(key: string) {
    return this.exec("credRead", key);
  }
  credDel(key: string) {
    return this.exec("credDel", key);
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
