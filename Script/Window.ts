import { Eventer } from "./Eventer";

export class Window extends Eventer {
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
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "window",
      srcId: globalThis.__HORSE_ID,
      tarId: this.id,
      methodName,
      params,
    });
  }
}
