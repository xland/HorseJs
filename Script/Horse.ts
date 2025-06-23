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
    return this.exec("getConfig");
  }
  getExeVer() {
    return this.exec("getExeVer");
  }
  getHorseVer() {
    return this.exec("getHorseVer");
  }
  quit() {
    return this.exec("quit");
  }
  relaunch() {
    return this.exec("relaunch");
  }
  exit(code: number) {
    return this.exec("exit", code);
  }
  async createWin(config: object) {
    let obj = await this.exec("createWin", config);
    return new WinProx(obj.id);
  }
  enableSecondIns() {
    return this.exec("enableSecondIns");
  }
  disableSecondIns() {
    return this.exec("disableSecondIns");
  }
  pack(exePath: string) {
    return this.exec("pack", exePath);
  }
  saveRes(resPath: string, tarPath: string, inDataDir = true) {
    return this.exec("saveRes", resPath, tarPath, inDataDir);
  }
  on(eventName, func) {
    if (eventName === "newWin") return this.onNewWin(func);
    let flag = this.listen(eventName, func);
    if (flag) {
      this.exec("on", eventName);
    }
  }
  off(eventName, func) {
    if (eventName === "newWin") return this.offNewWin(func);
    let flag = this.unlisten(eventName, func);
    if (flag) {
      this.exec("off", eventName);
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
  private exec(methodName: string, ...params: any[]) {
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
      this.exec("on", "newWin");
    }
  }
  private offNewWin(func) {
    let flag = this.unlisten("_newWin", func);
    if (flag) {
      this.unlisten("newWin");
      this.exec("off", "newWin");
    }
  }
}
if (window.self === window.top) {
  globalThis.horse = new Horse();
} else {
  globalThis.horse = window.top.horse;
}
