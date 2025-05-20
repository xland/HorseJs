(() => {
  // Types.ts
  var Horse;
  ((Horse3) => {
    let EventType;
    ((EventType2) => {
      EventType2[EventType2["Sys"] = 0] = "Sys";
      EventType2[EventType2["App"] = 1] = "App";
      EventType2[EventType2["Window"] = 2] = "Window";
      EventType2[EventType2["Page"] = 3] = "Page";
    })(EventType = Horse3.EventType || (Horse3.EventType = {}));
  })(Horse || (Horse = {}));

  // Util.ts
  var Util = class {
    postMsg(json) {
      window.chrome.webview.postMessage(json);
    }
    //生成一个12位的随机数字
    randomNum(len = 12) {
      return Math.floor(Math.pow(10, len) * Math.random());
    }
  };
  var util = new Util();

  // Eventer.ts
  var Eventer = class {
    dic = {};
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
      debugger;
      this.on(eventName, wrapper);
    }
    // 调用原生方法并返回 Promise
    call(msgType, msgName, ...params) {
      return new Promise((resolve, reject) => {
        const eventId = `e${util.randomNum()}`;
        this.once(eventId, (result) => {
          resolve(result);
        });
        window.chrome.webview.postMessage({ msgType, msgName, eventId, params });
      });
    }
  };
  var eventer = new Eventer();

  // Window.ts
  var Window = class {
    on(eventName, callback) {
      eventer.on(`window_${eventName}`, callback);
    }
    maximize() {
      return eventer.call(Horse.EventType.Window, "maximize");
    }
    minimize() {
      return eventer.call(Horse.EventType.Window, "minimize");
    }
    resize(w, h) {
      return eventer.call(Horse.EventType.Window, "resize", w, h);
    }
    move(x, y) {
      return eventer.call(Horse.EventType.Window, "resize", x, y);
    }
  };

  // Horse.ts
  var Horse2 = class {
    window;
    constructor() {
      this.window = new Window();
    }
  };
  globalThis.horse = new Horse2();
})();
