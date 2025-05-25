import { Window } from "./Window";
import { ClassId } from "./EnumId";
import { Fs } from "./Fs";
class Horse {
  window: Window;
  fs: Fs;
  webview = window.chrome.webview;
  constructor() {
    this.window = new Window(); // 初始化 window 实例
    this.fs = new Fs();
    this.listenMsg();
  }
  private listenMsg() {
    this.webview.addEventListener("message", (e) => {
      if (e.data.classId === ClassId.Window) {
        if (e.data.param) {
          this.window.emit(e.data.eventId, ...e.data.param);
        } else {
          this.window.emit(e.data.eventId, []);
        }
      }
    });
  }
}
console.log(123);
globalThis.horse = new Horse();
