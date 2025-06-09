import { Win } from "./Win";

export class WinProx extends Win {
  id: number;
  parent: Win;
  constructor(id: number, parent: Win) {
    super();
    this.id = id;
    this.parent = parent;
  }
  protected callMethod(methodName: string, ...params: any[]) {
    let obj = window.self === window.top ? this.parent : window.top.horse.win;
    return obj.call({
      className: "win",
      winId: globalThis.__WIN_ID,
      tarId: this.id,
      methodName,
      params,
    });
  }
}
