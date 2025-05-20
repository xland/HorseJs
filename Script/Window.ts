import { Eventer } from "./Eventer";
import { WindowMethodId, ClassId } from "./EnumId";

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
}
