import { util } from "./Util";
type EventHandler = (result: any) => void;
export class Eventer {
  id: number;
  private dic = {};
  constructor() {
    this.id = globalThis.__HORSE_ID;
  }
  // 监听事件
  on(eventName: string, callback: EventHandler) {
    if (!this.dic[eventName]) {
      this.dic[eventName] = [callback];
      if (!eventName.startsWith("once_")) {
        return this.call({
          className: "event",
          srcId: globalThis.__HORSE_ID,
          tarId: this.id,
          methodName: "on",
          params: [eventName],
        });
      }
    } else {
      this.dic[eventName].push(callback);
    }
  }

  // 发射事件
  emit(eventName: string, result: any): void {
    const handlers = this.dic[eventName];
    if (!handlers || handlers.length === 0) {
      console.warn(`没有找到该事件的监听函数：${eventName}`);
      return;
    }
    handlers.forEach((handler) => {
      handler(result);
    });
  }

  // 取消监听事件
  off(eventName: string, callback?: EventHandler): void {
    const handlers = this.dic[eventName];
    if (!handlers) return;
    if (!callback) {
      delete this.dic[eventName];
      return;
    }
    const index = handlers.findIndex((h) => h === callback);
    if (index >= 0) handlers.splice(index, 1);
    if (handlers.length === 0) delete this.dic[eventName];
  }

  // 监听一次性事件
  once(eventName: string, callback: EventHandler): void {
    const wrapper = (result: any) => {
      this.off(eventName, wrapper);
      callback(result);
    };
    this.on(eventName, wrapper);
  }
  // 调用原生方法并返回 Promise
  call(obj: any) {
    return new Promise((resolve, reject) => {
      obj.eventName = `once_${util.randomNum()}`;
      this.once(obj.eventName, (result: any) => {
        if (result.err) {
          reject(new Error(result.err));
        } else {
          resolve(result);
        }
      });
      window.chrome.webview.postMessage(obj);
    });
  }
}
