import { util } from "./Util";
type EventHandler = (...args: any[]) => void;
export class Eventer {
  private dic = {};
  constructor() {}
  // 监听事件
  on(methodId: number, callback: EventHandler): void {
    if (!this.dic[methodId]) {
      this.dic[methodId] = [callback];
    } else {
      this.dic[methodId].push(callback);
    }
  }

  // 发射事件
  emit(methodId: number, ...args: any[]): void {
    const handlers = this.dic[methodId];
    if (!handlers || handlers.length === 0) {
      console.warn(`没有找到该事件的监听函数：${methodId}`);
      return;
    }
    handlers.forEach((handler) => handler(...args));
  }

  // 取消监听事件
  off(methodId: number, callback?: EventHandler): void {
    const handlers = this.dic[methodId];
    if (!handlers) return;
    if (!callback) {
      delete this.dic[methodId];
      return;
    }
    const index = handlers.findIndex((h) => h === callback);
    if (index >= 0) handlers.splice(index, 1);
    if (handlers.length === 0) delete this.dic[methodId];
  }

  // 监听一次性事件
  once(methodId: number, callback: EventHandler): void {
    const wrapper = (...args: any[]) => {
      this.off(methodId, wrapper);
      callback(...args);
    };
    this.on(methodId, wrapper);
  }
  // 调用原生方法并返回 Promise
  call<T = any>(classId: number, methodId: number, ...params: any[]): Promise<T> {
    return new Promise((resolve, reject) => {
      const eventId = util.randomNum();
      this.once(eventId, (result: T) => {
        resolve(result);
      });
      debugger;
      window.chrome.webview.postMessage({ classId, methodId, eventId, params });
    });
  }
}
