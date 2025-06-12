import { Eventer } from "./Eventer";
import { util } from "./Util";
export class Tray extends Eventer {
  async create(config: any) {
    config.__id = util.randomNum();
    this.listen(config.__id, (data) => {
      let type = data.type;
      delete data.type;
      config[type]();
    });
    config.menu.forEach((item) => {
      item.__id = util.randomNum();
      this.unlisten(item.__id, item.click);
    });
    return this.exec("create", config);
  }
  private exec(methodName: string, ...params: any[]) {
    return this.call({
      className: "tray",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
