import { Eventer } from "./Eventer";

export class Win extends Eventer {
  id: number;
  pid = -1;
  create(config: object) {
    return this.execute("create", config);
  }
  sendMsg(tarId: number, msg: any) {
    return this.execute("sendMsg", tarId, msg);
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
  flash(flag = true) {
    return this.execute("flash", flag);
  }
  activate() {
    return this.execute("activate");
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
      winId: this.id,
      methodName,
      params,
    });
  }
}
