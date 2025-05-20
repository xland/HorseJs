import { Horse } from "./Types";
import { eventer } from "./Eventer";

export class Window {
  on(eventName: string, callback: Horse.EventHandler): void {
    eventer.on(`window_${eventName}`, callback);
  }
  maximize() {
    return eventer.call("window", "maximize");
  }
  minimize() {
    return eventer.call("window", "minimize");
  }
  resize(w: number, h: number) {
    return eventer.call("window", "resize", w, h);
  }
  move(x: number, y: number) {
    return eventer.call("window", "resize", x, y);
  }
}
