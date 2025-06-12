import { Eventer } from "./Eventer";
export class Dll extends Eventer {
  load(path: string) {
    return this.exec("load", path);
  }
  free() {
    return this.exec("free");
  }
  invoke(text: string, methodName: string, param: string) {
    return this.exec("invoke", text, methodName, param);
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
