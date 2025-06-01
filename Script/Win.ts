import { Eventer } from "./Eventer";

export class Win extends Eventer {
  maximize() {
    return this.callMethod("maximize");
  }
  minimize() {
    return this.callMethod("minimize");
  }
  hide() {
    return this.callMethod("hide");
  }
  show() {
    return this.callMethod("show");
  }
  restore() {
    return this.callMethod("restore");
  }
  resize(w: number, h: number) {
    return this.callMethod("resize", w, h);
  }
  move(x: number, y: number) {
    return this.callMethod("move", x, y);
  }
  close() {
    return this.callMethod("close");
  }
  destroy() {
    return this.callMethod("destroy");
  }
  flash(flag: boolean) {
    return this.callMethod("flash", flag);
  }
  startDrag() {
    return this.callMethod("startDrag");
  }
  setResizable(flag: boolean) {
    return this.callMethod("setResizable", flag);
  }
  openWindow(config: object) {
    return this.callMethod("openWindow", config);
  }
  addEventListener(eventName, func) {
    let flag = this.on(eventName, func);
    if (flag) {
      this.callMethod("addEventListener", eventName);
    }
  }
  removeEventListener(eventName, func) {
    let flag = this.off(eventName, func);
    if (flag) {
      this.callMethod("removeEventListener", eventName);
    }
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "win",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
