import { Eventer } from "./Eventer";
import { util } from "./Util";
export class Tray extends Eventer {
  async create(config: any) {
    config.id = util.randomNum(); //给它附加一个id
    if (config.rightBtnDown) {
      this.on("trayRightBtnDown", config.rightBtnDown);
      delete config.rightBtnDown;
    }
    if (config.leftBtnDown) {
      this.on("trayLeftBtnDown", config.leftBtnDown);
      delete config.leftBtnDown;
    }
    let i = 0;
    config.menu.forEach((item) => {
      if (item.click) {
        this.on("trayMenuClick" + i, config.leftBtnDown);
        delete config.leftBtnDown;
      }
    });
    return this.callMethod("create");
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "create",
      winId: globalThis.__WIN_ID,
      tarId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
