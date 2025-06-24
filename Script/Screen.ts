import { Eventer } from "./Eventer";
export class Screen extends Eventer {
  async getAll() {
    return this.exec("getAll");
  }
  async getDesktop() {
    return this.exec("getDesktop");
  }
  async getColor(x: number, y: number) {
    return this.exec("getColor", x, y);
  }
  async getImg(x: number, y: number, w: number, h: number) {
    return this.exec("getImg", x, y, w, h);
  }
  private exec(methodName: string, ...params: any[]) {
    return this.call({
      className: "screen",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
