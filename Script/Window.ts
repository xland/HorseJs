import { Horse } from "./Types";
import { eventer } from "./Eventer";

export class Window {
  on(eventName: string, callback: Horse.EventHandler): void {
    eventer.on(`window_${eventName}`, callback);
  }
  maximize() {
    return eventer.call(Horse.EventType.Window, "maximize");
  }
  minimize() {
    return eventer.call(Horse.EventType.Window, "minimize");
  }
  resize(w: number, h: number) {
    return eventer.call(Horse.EventType.Window, "resize", w, h);
  }
  move(x: number, y: number) {
    return eventer.call(Horse.EventType.Window, "resize", x, y);
  }
}
