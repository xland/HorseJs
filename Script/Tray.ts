import { Eventer } from "./Eventer";
import { util } from "./Util";
export class Tray extends Eventer {
  async create(config: any) {
    config.__id = util.randomNum();
    this.on(config.__id, (data) => {
      let type = data.type;
      delete data.type;
      config[type]();
    });
    config.menu.forEach((item) => {
      item.__id = util.randomNum();
      this.on(item.__id, item.click);
    });
    return this.callMethod("create", config);
  }
  private callMethod(methodName: string, ...params: any[]) {
    let obj = window.self === window.top ? this : window.top.horse.tray;
    return obj.call({
      className: "tray",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
