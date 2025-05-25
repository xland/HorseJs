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
    on(eventId, callback) {
      if (!this.dic[eventId]) {
        this.dic[eventId] = [callback];
      } else {
        this.dic[eventId].push(callback);
      }
    }
    // 发射事件
    emit(eventId, ...args) {
      const handlers = this.dic[eventId];
      if (!handlers || handlers.length === 0) {
        console.warn(`\u6CA1\u6709\u627E\u5230\u8BE5\u4E8B\u4EF6\u7684\u76D1\u542C\u51FD\u6570\uFF1A${eventId}`);
        return;
      }
      handlers.forEach((handler) => handler(...args));
    }
    // 取消监听事件
    off(eventId, callback) {
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
    once(eventId, callback) {
      const wrapper = (...args) => {
        this.off(eventId, wrapper);
        callback(...args);
      };
      this.on(eventId, wrapper);
    }
    // 调用原生方法并返回 Promise
    call(classId, methodId, ...params) {
      return new Promise((resolve, reject) => {
        const eventId = util.randomNum();
        this.once(eventId, (result) => {
          resolve(result);
        });
        window.chrome.webview.postMessage({ classId, methodId, eventId, params });
      });
    }
  };

  // EnumId.ts
  var WindowEventId = /* @__PURE__ */ ((WindowEventId2) => {
    WindowEventId2[WindowEventId2["closing"] = 0] = "closing";
    return WindowEventId2;
  })(WindowEventId || {});

  // Window.ts
  var Window = class extends Eventer {
    maximize() {
      return this.call(3 /* Window */, 0 /* maximize */);
    }
    minimize() {
      return this.call(3 /* Window */, 1 /* minimize */);
    }
    hide() {
      return this.call(3 /* Window */, 2 /* hide */);
    }
    show() {
      return this.call(3 /* Window */, 3 /* show */);
    }
    restore() {
      return this.call(3 /* Window */, 4 /* restore */);
    }
    resize(w, h) {
      return this.call(3 /* Window */, 5 /* resize */, w, h);
    }
    move(x, y) {
      return this.call(3 /* Window */, 6 /* move */, x, y);
    }
    close() {
      return this.call(3 /* Window */, 7 /* close */);
    }
    destroy() {
      return this.call(3 /* Window */, 8 /* destory */);
    }
    removeEventListener(eventName, cb) {
      if (!(eventName in WindowEventId)) return;
      let eId = WindowEventId[eventName];
      this.off(eId, cb);
      return this.call(3 /* Window */, 10 /* unregEvent */, eId);
    }
    addEventListener(eventName, cb) {
      if (!(eventName in WindowEventId)) return;
      let eId = WindowEventId[eventName];
      this.on(eId, cb);
      return this.call(3 /* Window */, 9 /* regEvent */, eId);
    }
  };

  // Fs.ts
  var Fs = class extends Eventer {
    addResToExe(dirPath, exePath) {
      return this.call(1 /* Fs */, 0 /* addResToExe */, dirPath, exePath);
    }
  };

  // Horse.ts
  var Horse = class {
    window;
    fs;
    webview = window.chrome.webview;
    constructor() {
      this.window = new Window();
      this.fs = new Fs();
      this.listenMsg();
    }
    listenMsg() {
      this.webview.addEventListener("message", (e) => {
        if (e.data.classId === 3 /* Window */) {
          if (e.data.param) {
            this.window.emit(e.data.eventId, ...e.data.param);
          } else {
            this.window.emit(e.data.eventId, []);
          }
        }
      });
    }
  };
  console.log(123);
  globalThis.horse = new Horse();
})();
