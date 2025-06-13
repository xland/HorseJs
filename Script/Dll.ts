import { Eventer } from "./Eventer";
export class Dll extends Eventer {
  load(path: string) {
    return this.exec("load", path);
  }
  free(id: number) {
    return this.exec("free", id);
  }
  invoke(id: number, methodName: string, param: string) {
    return this.exec("invoke", id, methodName, param);
  }
  on(id: number, eventName: string, cb: (data) => {}) {
    if (this.listen(eventName, cb)) {
      return this.exec("on", id, eventName);
    }
  }
  off(id: number, eventName: string, cb: (data) => {}) {
    if (this.unlisten(eventName, cb)) {
      return this.exec("off", id, eventName);
    }
  }
  private exec(methodName: string, ...params: any[]) {
    return this.call({
      className: "dll",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
