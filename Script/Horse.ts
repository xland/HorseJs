import { Window } from "./Window";
import { ClassId, HorseMethodId } from "./EnumId";
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
    return this.call(ClassId.Horse, HorseMethodId.getConfig);
  }
  private listenMsg() {
    this.webview.addEventListener("message", (e) => {
      if (e.data.classId === ClassId.Horse) {
        this.emit(e.data.eventId, e.data.data);
      } else if (e.data.classId === ClassId.Window) {
        this.window.emit(e.data.eventId, e.data.data);
      }
    });
  }
}
globalThis.horse = new Horse();
