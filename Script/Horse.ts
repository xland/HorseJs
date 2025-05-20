import { Window } from "./Window";

class Horse {
  window: Window;
  constructor() {
    this.window = new Window(); // 初始化 window 实例
  }
}

globalThis.horse = new Horse();
