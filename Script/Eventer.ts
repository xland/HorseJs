export class Eventer {
  private dic: Record<string, EventHandler[]> = {};
  constructor() {
    window.chrome.webview.addEventListener("message", (e) => {
      let arr = this.dic[e.data["eventName"]];
      for (let i = 0; i < arr.length; i++) {
        arr[i](...e.data["param"]);
      }
    });
  }
  // 监听事件
  on(eventName: string, callback: EventHandler): void {
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
    const wrapper = (...args: any[]) => {
      this.off(eventName, wrapper);
      callback(...args);
    };
    this.on(eventName, wrapper);
  }
  //生成一个12位的随机数字
  private randomNum(len: number = 12): number {
    return Math.floor(Math.pow(10, len) * Math.random());
  }
  // 调用原生方法并返回 Promise
  call<T = any>(msgName: string, ...params: any[]): Promise<T> {
    return new Promise((resolve, reject) => {
      const eventName = `${msgName}_${this.randomNum()}`;
      this.once(eventName, (result: T) => {
        resolve(result);
      });
      window.chrome.webview.postMessage({ eventName, ...params });
    });
  }
}
