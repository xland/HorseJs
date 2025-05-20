import { Horse } from "./Types";
import { util } from "./Util";
class Eventer {
  private dic: Record<string, Horse.EventHandler[]> = {};
  constructor() {
    window.chrome.webview.addEventListener("message", (e) => {
      debugger;
      let arr = this.dic[e.data["eventId"]];
      if (!arr) return;
      for (let i = 0; i < arr.length; i++) {
        arr[i](...e.data["param"]);
      }
    });
  }
  // 监听事件
  on(eventName: string, callback: Horse.EventHandler): void {
    if (!this.dic[eventName]) {
      this.dic[eventName] = [callback];
    } else {
      this.dic[eventName].push(callback);
    }
  }

  // 发射事件
  emit(eventName: string, ...args: any[]): void {
    const handlers = this.dic[eventName];
    if (!handlers || handlers.length === 0) {
      console.warn(`没有找到该事件的监听函数：${eventName}`);
      return;
    }
    handlers.forEach((handler) => handler(...args));
  }

  // 取消监听事件
  off(eventName: string, callback?: Horse.EventHandler): void {
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
  once(eventName: string, callback: Horse.EventHandler): void {
    const wrapper = (...args: any[]) => {
      this.off(eventName, wrapper);
      callback(...args);
    };
    debugger;
    this.on(eventName, wrapper);
  }
  // 调用原生方法并返回 Promise
  call<T = any>(msgType: string, msgName: string, ...params: any[]): Promise<T> {
    return new Promise((resolve, reject) => {
      const eventId = `e${util.randomNum()}`;
      this.once(eventId, (result: T) => {
        resolve(result);
      });
      window.chrome.webview.postMessage({ msgType, msgName, eventId, params });
    });
  }
}

export let eventer = new Eventer();
