import { Window } from "./Window";
import { ClassId } from "./EnumId";
class Horse {
  window: Window;
  webview = window.chrome.webview;
  constructor() {
    this.window = new Window(); // 初始化 window 实例
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

globalThis.horse = new Horse();
