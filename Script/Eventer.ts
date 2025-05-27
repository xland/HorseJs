import { util } from "./Util";
type EventHandler = (result: any) => void;
export class Eventer {
  private dic = {};
  constructor() {}
  // 监听事件
  on(eventId: number, callback: EventHandler): void {
    if (!this.dic[eventId]) {
      this.dic[eventId] = [callback];
    } else {
      this.dic[eventId].push(callback);
    }
  }

  // 发射事件
  emit(eventId: number, result: any): void {
    const handlers = this.dic[eventId];
    if (!handlers || handlers.length === 0) {
      console.warn(`没有找到该事件的监听函数：${eventId}`);
      return;
    }
    handlers.forEach((handler) => {
      handler(result);
    });
  }

  // 取消监听事件
  off(eventId: number, callback?: EventHandler): void {
    const handlers = this.dic[eventId];
    if (!handlers) return;
    if (!callback) {
      delete this.dic[eventId];
      return;
    }
    const index = handlers.findIndex((h) => h === callback);
    if (index >= 0) handlers.splice(index, 1);
    if (handlers.length === 0) delete this.dic[eventId];
  }

  // 监听一次性事件
  once(eventId: number, callback: EventHandler): void {
    const wrapper = (result: any) => {
      this.off(eventId, wrapper);
      callback(result);
    };
    this.on(eventId, wrapper);
  }
  // 调用原生方法并返回 Promise
  call(classId: number, methodId: number, ...params: any[]) {
    return new Promise((resolve, reject) => {
      const eventId = util.randomNum();
      this.once(eventId, (result: any) => {
        if (result.err) {
          reject(new Error(result.err));
        } else {
          resolve(result);
        }
      });
      window.chrome.webview.postMessage({ classId, methodId, eventId, params });
    });
  }
}
