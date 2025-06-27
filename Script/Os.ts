import { Eventer } from "./Eventer";
import { util } from "./Util";
export class Os extends Eventer {
  getOsVer() {
    return this.execute("getOsVer");
  }
  getCpuId() {
    return this.execute("getCpuId");
  }
  getDiskId() {
    return this.execute("getDiskId");
  }
  getUserLang() {
    return this.execute("getUserLang");
  }
  getOsLang() {
    return this.execute("getOsLang");
  }
  getOsColor() {
    return this.execute("getOsColor");
  }
  preventSleep() {
    return this.execute("preventSleep");
  }
  stopPreventSleep() {
    return this.execute("stopPreventSleep");
  }
  getIpAddr() {
    return this.execute("getIpAddr");
  }
  showNotify(appName: string, title: string, content: string) {
    return this.execute("showNotify", appName, title, content);
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
    return this.execute("createTray", config);
  }
  destroyTray(id: number) {
    return this.execute("destroyTray", id);
  }
  spawn(path: string) {
    return this.execute("spawn", path);
  }
  creadWrite(key: string, val: string) {
    return this.execute("credWrite", key, val);
  }
  credRead(key: string) {
    return this.execute("credRead", key);
  }
  credDel(key: string) {
    return this.execute("credDel", key);
  }
  on(eventName, func) {
    let flag = this.listen(eventName, func);
    if (flag) {
      this.execute("on", eventName);
    }
  }
  off(eventName, func) {
    let flag = this.unlisten(eventName, func);
    if (flag) {
      this.execute("off", eventName);
    }
  }
  private execute(methodName: string, ...params: any[]) {
    return this.call({
      className: "os",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
