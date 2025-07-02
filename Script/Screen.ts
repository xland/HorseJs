import { Eventer } from "./Eventer";
export class Screen extends Eventer {
  async getAll() {
    return this.execute("getAll");
  }
  async getDesktop() {
    return this.execute("getDesktop");
  }
  async getColor(x: number, y: number) {
    return this.execute("getColor", x, y);
  }
  async getImg(x: number, y: number, w: number, h: number) {
    return this.execute("getImg", x, y, w, h);
  }
  private execute(methodName: string, ...params: any[]) {
    return this.call({
      className: "screen",
      winId: horse.win.id,
      methodName,
      params,
    });
  }
}
