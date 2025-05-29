import { util } from "./Util";
import { Eventer } from "./Eventer";
export class Fs extends Eventer {
  readFile(filePath: string, func) {
    let eventName = `cb_${util.randomNum()}`;
    this.on(eventName, func);
    this.callMethod("readFile", filePath, eventName);
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "fs",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
