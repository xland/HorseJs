import { Win } from "./Win";

export class WinProx extends Win {
  id: number;
  constructor(id: number) {
    super();
    this.id = id;
  }
  protected execute(methodName: string, ...params: any[]) {
    return horse.win.call({
      className: "win",
      winId: globalThis.__WIN_ID,
      tarId: this.id,
      methodName,
      params,
    });
  }
}
