import { Window } from "./Window";
import { ClassId } from "./EnumId";
class Horse {
  window: Window;
  constructor() {
    this.window = new Window(); // 初始化 window 实例
    this.listenMsg();
  }
  private listenMsg() {
    window.chrome.webview.addEventListener("message", (e) => {
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
