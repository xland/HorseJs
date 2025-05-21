import { Eventer } from "./Eventer";
import { WindowMethodId, ClassId, WindowEventId } from "./EnumId";

export class Window extends Eventer {
  maximize() {
    return this.call(ClassId.Window, WindowMethodId.maximize);
  }
  minimize() {
    return this.call(ClassId.Window, WindowMethodId.minimize);
  }
  resize(w: number, h: number) {
    return this.call(ClassId.Window, WindowMethodId.resize, w, h);
  }
  move(x: number, y: number) {
    return this.call(ClassId.Window, WindowMethodId.move, x, y);
  }
  removeEventListener(eventName: string, cb: any) {
    if (!(eventName in WindowEventId)) return;
    let eId = WindowEventId[eventName as keyof typeof WindowEventId] as WindowEventId;
    this.off(eId, cb);
    return this.call(ClassId.Window, WindowMethodId.unregEvent, eId);
  }
  addEventListener(eventName: string, cb: (...args: any[]) => void) {
    if (!(eventName in WindowEventId)) return;
    let eId = WindowEventId[eventName as keyof typeof WindowEventId] as WindowEventId;
    this.on(eId, cb);
    return this.call(ClassId.Window, WindowMethodId.regEvent, eId);
  }
}
