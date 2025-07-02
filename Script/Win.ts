import { Eventer } from "./Eventer";

export class Win extends Eventer {
  id: string;

  async create(config: object) {
    return this.execute("create", config);
  }
  maximize() {
    return this.execute("maximize");
  }
  minimize() {
    return this.execute("minimize");
  }
  hide() {
    return this.execute("hide");
  }
  show() {
    return this.execute("show");
  }
  restore() {
    return this.execute("restore");
  }
  resize(w: number, h: number) {
    return this.execute("resize", w, h);
  }
  move(x: number, y: number) {
    return this.execute("move", x, y);
  }
  close() {
    return this.execute("close");
  }
  destroy() {
    return this.execute("destroy");
  }
  flash(flag: boolean) {
    return this.execute("flash", flag);
  }
  startDrag() {
    return this.execute("startDrag");
  }
  setResizable(flag: boolean) {
    return this.execute("setResizable", flag);
  }
  on(eventName, func) {
    let flag = this.listen(eventName, func);
    if (flag) {
      return this.execute("on", eventName);
    }
    return Promise.resolve({ ok: true });
  }
  off(eventName, func) {
    let flag = this.unlisten(eventName, func);
    if (flag) {
      return this.execute("off", eventName);
    }
    return Promise.resolve({ ok: true });
  }
  protected execute(methodName: string, ...params: any[]) {
    return this.call({
      className: "win",
      winId: horse.win.id,
      methodName,
      params,
    });
  }
}
