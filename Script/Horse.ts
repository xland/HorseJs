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
        this.emit(e.data.eventId, e.data.data);
      } else if (e.data.className === "window") {
        this.window.emit(e.data.eventId, e.data.data);
      }
    });
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "horse",
      srcId: globalThis.__HORSE_ID,
      tarId: this.id,
      methodName,
      params,
    });
  }
}
globalThis.horse = new Horse();
