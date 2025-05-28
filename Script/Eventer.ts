import { util } from "./Util";
type EventHandler = (result: any) => void;
export class Eventer {
  private dic = {};
  constructor() {}
  // 监听事件
  protected on(eventName: string, callback: EventHandler) {
    if (!this.dic[eventName]) {
      this.dic[eventName] = [callback];
      return true;
    } else {
      this.dic[eventName].push(callback);
      return false;
    }
  }
  // 取消监听事件
  protected off(eventName: string, callback?: EventHandler) {
    const handlers = this.dic[eventName];
    if (!handlers) return false;
    if (callback) {
      const index = handlers.findIndex((h) => h === callback);
      if (index >= 0) handlers.splice(index, 1);
    }
    if (handlers.length === 0 || !callback) {
      delete this.dic[eventName];
      return true;
    }
    return false;
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
