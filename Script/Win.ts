import { Eventer } from "./Eventer";

export class Win extends Eventer {
  maximize() {
    return this.exec("maximize");
  }
  minimize() {
    return this.exec("minimize");
  }
  hide() {
    return this.exec("hide");
  }
  show() {
    return this.exec("show");
  }
  restore() {
    return this.exec("restore");
  }
  resize(w: number, h: number) {
    return this.exec("resize", w, h);
  }
  move(x: number, y: number) {
    return this.exec("move", x, y);
  }
  close() {
    return this.exec("close");
  }
  destroy() {
    return this.exec("destroy");
  }
  flash(flag: boolean) {
    return this.exec("flash", flag);
  }
  startDrag() {
    return this.exec("startDrag");
  }
  setResizable(flag: boolean) {
    return this.exec("setResizable", flag);
  }
  on(eventName, func) {
    let flag = this.listen(eventName, func);
    if (flag) {
      this.exec("on", eventName);
    }
  }
  off(eventName, func) {
    let flag = this.unlisten(eventName, func);
    if (flag) {
      this.exec("off", eventName);
    }
  }
  protected exec(methodName: string, ...params: any[]) {
    return this.call({
      className: "win",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
