import { Eventer } from "./Eventer";
import { Window } from "./Window";

class Horse extends Eventer {
  window: Window;
  constructor() {
    super(); // 调用父类构造函数
    this.window = new Window(); // 初始化 window 实例
  }
}

globalThis.horse = new Horse();
