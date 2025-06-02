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
    return this.parent.call({
      className: "win",
      winId: globalThis.__WIN_ID,
      tarId: this.id,
      methodName,
      params,
    });
  }
}
