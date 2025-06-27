import { Dll } from "./Dll";
import { WinProx } from "./WinProxy";
import { Win } from "./Win";
import { Fs } from "./Fs";
import { Eventer } from "./Eventer";
import { Dialog } from "./Dialog";
import { Clipboard } from "./Clipboard";
import { Os } from "./Os";
import { Screen } from "./Screen";
import { Db } from "./Db";

class Horse extends Eventer {
  clipboard: Clipboard;
  db: Db;
  dialog: Dialog;
  dll: Dll;
  fs: Fs;
  os: Os;
  screen: Screen;
  win: Win;
  webview;
  constructor() {
    super();
    this.webview = window.chrome.webview;
    this.win = new Win();
    this.fs = new Fs();
    this.dialog = new Dialog();
    this.clipboard = new Clipboard();
    this.os = new Os();
    this.screen = new Screen();
    this.dll = new Dll();
    this.db = new Db();
    this.listenMsg();
  }
  getConfig() {
    return this.execute("getConfig");
  }
  getExeVer() {
    return this.execute("getExeVer");
  }
  getHorseVer() {
    return this.execute("getHorseVer");
  }
  quit() {
    return this.execute("quit");
  }
  relaunch() {
    return this.execute("relaunch");
  }
  exit(code: number) {
    return this.execute("exit", code);
  }
  async createWin(config: object) {
    let obj = await this.execute("createWin", config);
    return new WinProx(obj.id);
  }
  enableSecondIns() {
    return this.execute("enableSecondIns");
  }
  disableSecondIns() {
    return this.execute("disableSecondIns");
  }
  packRes(exePath: string) {
    return this.execute("packRes", exePath);
  }
  saveRes(resPath: string, tarPath = resPath, inDataDir = true) {
    return this.execute("saveRes", resPath, tarPath, inDataDir);
  }
  autoStart(flag = true) {
    return this.execute("autoStart", flag);
  }
  on(eventName, func) {
    if (eventName === "newWin") return this.onNewWin(func);
    let flag = this.listen(eventName, func);
    if (flag) {
      this.execute("on", eventName);
    }
  }
  off(eventName, func) {
    if (eventName === "newWin") return this.offNewWin(func);
    let flag = this.unlisten(eventName, func);
    if (flag) {
      this.execute("off", eventName);
    }
  }

  private listenMsg() {
    this.webview.addEventListener("message", (e) => {
      let clsName = e.data.className;
      delete e.data.className;
      let evtName = e.data.eventName;
      delete e.data.eventName;
      if (clsName !== "horse") {
        this[clsName].emit(evtName, e.data);
      } else {
        this.emit(evtName, e.data);
      }
    });
    this.webview.addEventListener("sharedbufferreceived", (e) => {
      const buffer = e.getBuffer();
      let clsName = e.additionalData.className;
      delete e.additionalData.className;
      let evtName = e.additionalData.eventName;
      delete e.additionalData.eventName;
      if (clsName !== "horse") {
        this[clsName].emit(evtName, {
          buffer,
          ...e.additionalData,
          release: () => {
            window.chrome.webview.releaseBuffer(buffer);
          },
        });
      } else {
        this.emit(evtName, {
          buffer,
          ...e.additionalData,
          release: () => {
            window.chrome.webview.releaseBuffer(buffer);
          },
        });
      }
    });
  }
  private execute(methodName: string, ...params: any[]) {
    return this.call({
      className: "horse",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
  private onNewWin(func) {
    this.listen("_newWin", func);
    if (!this.has("newWin")) {
      this.listen("newWin", (data) => {
        let proxy = new WinProx(data.id);
        this.emit("_newWin", proxy);
      });
      this.execute("on", "newWin");
    }
  }
  private offNewWin(func) {
    let flag = this.unlisten("_newWin", func);
    if (flag) {
      this.unlisten("newWin");
      this.execute("off", "newWin");
    }
  }
}
if (window.self === window.top) {
  globalThis.horse = new Horse();
} else {
  globalThis.horse = window.top.horse;
}
