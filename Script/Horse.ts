import { Window } from "./Window";
import { Fs } from "./Fs";
import { Eventer } from "./Eventer";
class Horse extends Eventer {
  window: Window;
  fs: Fs;
  webview;
  constructor() {
    super();
    this.webview = window.chrome.webview;
    this.window = new Window(); // 初始化 window 实例
    this.fs = new Fs();
    this.listenMsg();
  }
  getConfig() {
    return this.callMethod("getConfig");
  }
  private listenMsg() {
    this.webview.addEventListener("message", (e) => {
      if (e.data.className === "horse") {
        this.emit(e.data.eventName, e.data);
      } else if (e.data.className === "window") {
        this.window.emit(e.data.eventName, e.data);
      } else if (e.data.className === "fs") {
        this.fs.emit(e.data.eventName, e.data);
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
