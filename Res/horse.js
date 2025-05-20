(() => {
  // Util.ts
  var Util = class {
    postMsg(json) {
      window.chrome.webview.postMessage(json);
    }
    //生成一个12位的随机数字
    randomNum(len = 9) {
      return Math.floor(Math.pow(10, len) * Math.random());
    }
  };
  var util = new Util();

  // Eventer.ts
  var Eventer = class {
    dic = {};
    constructor() {
    }
    // 监听事件
    on(methodId, callback) {
      if (!this.dic[methodId]) {
        this.dic[methodId] = [callback];
      } else {
        this.dic[methodId].push(callback);
      }
    }
    // 发射事件
    emit(methodId, ...args) {
      const handlers = this.dic[methodId];
      if (!handlers || handlers.length === 0) {
        console.warn(`\u6CA1\u6709\u627E\u5230\u8BE5\u4E8B\u4EF6\u7684\u76D1\u542C\u51FD\u6570\uFF1A${methodId}`);
        return;
      }
      handlers.forEach((handler) => handler(...args));
    }
    // 取消监听事件
    off(methodId, callback) {
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
    once(methodId, callback) {
      const wrapper = (...args) => {
        this.off(methodId, wrapper);
        callback(...args);
      };
      this.on(methodId, wrapper);
    }
    // 调用原生方法并返回 Promise
    call(classId, methodId, ...params) {
      return new Promise((resolve, reject) => {
        const eventId = util.randomNum();
        this.once(eventId, (result) => {
          resolve(result);
        });
        debugger;
        window.chrome.webview.postMessage({ classId, methodId, eventId, params });
      });
    }
  };

  // Window.ts
  var Window = class extends Eventer {
    maximize() {
      return this.call(2 /* Window */, 0 /* maximize */);
    }
    minimize() {
      return this.call(2 /* Window */, 1 /* minimize */);
    }
    resize(w, h) {
      return this.call(2 /* Window */, 2 /* resize */, w, h);
    }
    move(x, y) {
      return this.call(2 /* Window */, 3 /* move */, x, y);
    }
  };

  // Horse.ts
  var Horse = class {
    window;
    constructor() {
      this.window = new Window();
      this.listenMsg();
    }
    listenMsg() {
      window.chrome.webview.addEventListener("message", (e) => {
        if (e.data.classId === 2 /* Window */) {
          this.window.emit(e.data.eventId, ...e.data.param);
        }
      });
    }
  };
  globalThis.horse = new Horse();
})();
