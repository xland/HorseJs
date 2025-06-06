import { util } from "./Util";
import { Eventer } from "./Eventer";
export class Menu extends Eventer {
  async create(config: any) {
    let id = util.randomNum();
    config.menu.forEach((item) => {
      item.id = util.randomNum();
      if (item.click) {
        this.on(item.id, item.click);
        delete item.click;
      }
    });
    return this.callMethod("create", config, id);
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
