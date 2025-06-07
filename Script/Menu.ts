import { util } from "./Util";
import { Eventer } from "./Eventer";
export class Menu extends Eventer {
  async create(arr: any) {
    arr.forEach((item) => {
      item.__id = util.randomNum();
      if (item.click) {
        this.on(item.__id, item.click);
        delete item.click;
      }
    });
    return this.callMethod("create", ...arr);
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "menu",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
