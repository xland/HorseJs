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
    id;
    dic = {};
    constructor() {
      this.id = globalThis.__HORSE_ID;
    }
    // 监听事件
    on(eventName, callback) {
      if (!this.dic[eventName]) {
        this.dic[eventName] = [callback];
        if (!eventName.startsWith("once_")) {
          return this.call({
            className: "event",
            srcId: globalThis.__HORSE_ID,
            tarId: this.id,
            methodName: "on",
            params: [eventName]
          });
        }
      } else {
        this.dic[eventName].push(callback);
      }
    }
    // 发射事件
    emit(eventName, result) {
      const handlers = this.dic[eventName];
      if (!handlers || handlers.length === 0) {
        console.warn(`\u6CA1\u6709\u627E\u5230\u8BE5\u4E8B\u4EF6\u7684\u76D1\u542C\u51FD\u6570\uFF1A${eventName}`);
        return;
      }
      handlers.forEach((handler) => {
        handler(result);
      });
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
      const wrapper = (result) => {
        this.off(eventName, wrapper);
        callback(result);
      };
      this.on(eventName, wrapper);
    }
    // 调用原生方法并返回 Promise
    call(obj) {
      return new Promise((resolve, reject) => {
        obj.eventName = `once_${util.randomNum()}`;
        this.once(obj.eventName, (result) => {
          if (result.err) {
            reject(new Error(result.err));
          } else {
            resolve(result);
          }
        });
        window.chrome.webview.postMessage(obj);
      });
    }
  };

  // Window.ts
  var Window = class extends Eventer {
    maximize() {
      return this.callMethod("maximize");
    }
    minimize() {
      return this.callMethod("minimize");
    }
    hide() {
      return this.callMethod("hide");
    }
    show() {
      return this.callMethod("show");
    }
    restore() {
      return this.callMethod("restore");
    }
    resize(w, h) {
      return this.callMethod("resize", w, h);
    }
    move(x, y) {
      return this.callMethod("move", x, y);
    }
    close() {
      return this.callMethod("close");
    }
    destroy() {
      return this.callMethod("destroy");
    }
    flash(flag) {
      return this.callMethod("flash", flag);
    }
    startDrag() {
      return this.callMethod("startDrag");
    }
    setResizable(flag) {
      return this.callMethod("setResizable", flag);
    }
    openWindow(config) {
      return this.callMethod("openWindow", config);
    }
    callMethod(methodName, ...params) {
      return this.call({
        className: "window",
        srcId: globalThis.__HORSE_ID,
        tarId: this.id,
        methodName,
        params
      });
    }
  };

  // Fs.ts
  var Fs = class extends Eventer {
    addResToExe(dirPath, exePath) {
    }
  };

  // Horse.ts
  var Horse = class extends Eventer {
    window;
    fs;
    webview;
    constructor() {
      super();
      this.webview = window.chrome.webview;
      this.window = new Window();
      this.fs = new Fs();
      this.listenMsg();
    }
    getConfig() {
      return this.callMethod("getConfig");
    }
    listenMsg() {
      this.webview.addEventListener("message", (e) => {
        if (e.data.className === "horse") {
          this.emit(e.data.eventId, e.data.data);
        } else if (e.data.className === "window") {
          this.window.emit(e.data.eventId, e.data.data);
        }
      });
    }
    callMethod(methodName, ...params) {
      return this.call({
        className: "horse",
        srcId: globalThis.__HORSE_ID,
        tarId: this.id,
        methodName,
        params
      });
    }
  };
  globalThis.horse = new Horse();
})();
