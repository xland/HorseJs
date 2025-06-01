import { Win } from "./Win";
import { Fs } from "./Fs";
import { Eventer } from "./Eventer";
import { Dialog } from "./Dialog";
class Horse extends Eventer {
  win: Win;
  fs: Fs;
  dialog: Dialog;
  webview;
  constructor() {
    super();
    this.webview = window.chrome.webview;
    this.win = new Win(); // 初始化 window 实例
    this.fs = new Fs();
    this.dialog = new Dialog();
    this.listenMsg();
  }
  getConfig() {
    return this.callMethod("getConfig");
  }
  private listenMsg() {
    this.webview.addEventListener("message", (e) => {
      let clsName = e.data.className;
      delete e.data.className;
      let evtName = e.data.eventName;
      delete e.data.eventName;
      if (clsName === "horse") {
        this.emit(evtName, e.data);
      } else if (clsName === "win") {
        this.win.emit(evtName, e.data);
      } else if (clsName === "fs") {
        this.fs.emit(evtName, e.data);
      }
    });
    this.webview.addEventListener("sharedbufferreceived", (e) => {
      const buffer = e.getBuffer();
      let clsName = e.additionalData.className;
      delete e.additionalData.className;
      let evtName = e.additionalData.eventName;
      delete e.additionalData.eventName;
      if (clsName === "fs") {
        this.fs.emit(evtName, {
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
globalThis.horse = new Horse();
