(() => {
  // Eventer.ts
  var Eventer = class {
    dic = {};
    constructor() {
      window.chrome.webview.addEventListener("message", (e) => {
        let arr = this.dic[e.data["eventName"]];
        for (let i = 0; i < arr.length; i++) {
          arr[i](...e.data["param"]);
        }
      });
    }
    // 监听事件
    on(eventName, callback) {
      if (!this.dic[eventName]) {
        this.dic[eventName] = [callback];
      } else {
        this.dic[eventName].push(callback);
      }
    }
    // 发射事件
    emit(eventName, ...args) {
      const handlers = this.dic[eventName];
      if (!handlers || handlers.length === 0) {
        console.warn(`\u6CA1\u6709\u627E\u5230\u8BE5\u4E8B\u4EF6\u7684\u76D1\u542C\u51FD\u6570\uFF1A${eventName}`);
        return;
      }
      handlers.forEach((handler) => handler(...args));
    }
    // 取消监听事件
    off(eventName, callback) {
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
    once(eventName, callback) {
      const wrapper = (...args) => {
        this.off(eventName, wrapper);
        callback(...args);
      };
      this.on(eventName, wrapper);
    }
    //生成一个12位的随机数字
    randomNum(len = 12) {
      return Math.floor(Math.pow(10, len) * Math.random());
    }
    // 调用原生方法并返回 Promise
    call(msgName, ...params) {
      return new Promise((resolve, reject) => {
        const eventName = `${msgName}_${this.randomNum()}`;
        this.once(eventName, (result) => {
          resolve(result);
        });
        window.chrome.webview.postMessage({ eventName, ...params });
      });
    }
  };

  // Window.ts
  var Window = class extends Eventer {
    on(eventName, callback) {
      super.on(`window_${eventName}`, callback);
    }
  };

  // Horse.ts
  var Horse = class extends Eventer {
    window;
    constructor() {
      super();
      this.window = new Window();
    }
  };
  globalThis.horse = new Horse();
})();
