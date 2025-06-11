import { Process } from "./Process";
import { Tray } from "./Tray";
import { Notify } from "./Notify";
import { WinProx } from "./WinProxy";
import { Win } from "./Win";
import { Fs } from "./Fs";
import { Eventer } from "./Eventer";
import { Dialog } from "./Dialog";
import { Clipboard } from "./Clipboard";
import { Net } from "./Net";
import { Lib } from "./Lib";
import { Os } from "./Os";
import { Screen } from "./Screen";
class Horse extends Eventer {
  win: Win;
  fs: Fs;
  dialog: Dialog;
  clipboard: Clipboard;
  notify: Notify;
  lib: Lib;
  net: Net;
  os: Os;
  screen: Screen;
  tray: Tray;
  process: Process;
  webview;
  constructor() {
    super();
    this.webview = window.chrome.webview;
    this.win = new Win();
    this.fs = new Fs();
    this.dialog = new Dialog();
    this.clipboard = new Clipboard();
    this.net = new Net();
    this.notify = new Notify();
    this.os = new Os();
    this.screen = new Screen();
    this.tray = new Tray();
    this.process = new Process();
    this.listenMsg();
  }
  getConfig() {
    return this.callMethod("getConfig");
  }
  getVersion() {
    return this.callMethod("getVersion");
  }
  exit(code: number) {
    return this.callMethod("exit", code);
  }
  async createWindow(config: object) {
    let obj = await this.callMethod("createWindow", config);
    return new WinProx(obj.id, this.win);
  }
  private listenMsg() {
    if (window.self !== window.top) return;
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
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "horse",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
if (window.self === window.top) {
  globalThis.horse = new Horse();
} else {
  globalThis.horse = window.top.horse;
}
