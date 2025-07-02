import { Eventer } from "./Eventer";
export class Dll extends Eventer {
  load(path: string) {
    return this.execute("load", path);
  }
  free(id: number) {
    return this.execute("free", id);
  }
  invoke(id: number, methodName: string, param: string) {
    return this.execute("invoke", id, methodName, param);
  }
  on(id: number, eventName: string, cb: (data) => {}) {
    if (this.listen(eventName, cb)) {
      return this.execute("on", id, eventName);
    }
  }
  off(id: number, eventName: string, cb: (data) => {}) {
    if (this.unlisten(eventName, cb)) {
      return this.execute("off", id, eventName);
    }
  }
  private execute(methodName: string, ...params: any[]) {
    return this.call({
      className: "dll",
      winId: horse.win.id,
      methodName,
      params,
    });
  }
}
