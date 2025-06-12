import { Win } from "./Win";

export class WinProx extends Win {
  id: number;
  constructor(id: number, parent: Win) {
    super();
    this.id = id;
  }
  protected exec(methodName: string, ...params: any[]) {
    return this.call({
      className: "win",
      winId: globalThis.__WIN_ID,
      tarId: this.id,
      methodName,
      params,
    });
  }
}
