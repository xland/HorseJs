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
    on(eventName, callback) {
      if (!this.dic[eventName]) {
        this.dic[eventName] = [callback];
        return true;
      } else {
        this.dic[eventName].push(callback);
        return false;
      }
    }
    // 取消监听事件
    off(eventName, callback) {
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

  // Win.ts
  var Win = class extends Eventer {
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
    addEventListener(eventName, func) {
      let flag = this.on(eventName, func);
      if (flag) {
        this.callMethod("addEventListener", eventName);
      }
    }
    removeEventListener(eventName, func) {
      let flag = this.off(eventName, func);
      if (flag) {
        this.callMethod("removeEventListener", eventName);
      }
    }
    callMethod(methodName, ...params) {
      return this.call({
        className: "win",
        winId: globalThis.__WIN_ID,
        tarId: globalThis.__WIN_ID,
        methodName,
        params
      });
    }
  };

  // WinProxy.ts
  var WinProx = class extends Win {
    id;
    parent;
    constructor(id, parent) {
      super();
      this.id = id;
      this.parent = parent;
    }
    callMethod(methodName, ...params) {
      return this.parent.call({
        className: "win",
        winId: globalThis.__WIN_ID,
        tarId: this.id,
        methodName,
        params
      });
    }
  };

  // Fs.ts
  var Fs = class extends Eventer {
    async readFile(filePath) {
      return this.callMethod("readFile", filePath);
    }
    async readFileChunk(filePath, startPos, chunkSize) {
      return this.callMethod("readFileChunk", filePath, startPos, chunkSize);
    }
    async exists(filePath) {
      return this.callMethod("exists", filePath);
    }
    async getFileInfo(filePath) {
      return this.callMethod("getFileInfo", filePath);
    }
    async writeFile(filePath, content) {
      return this.callMethod("writeFile", filePath, content);
    }
    async writeFileChunk(filePath, content, startPos) {
      return this.callMethod("writeFileChunk", filePath, content, startPos);
    }
    async delPath(filePath) {
      return this.callMethod("delPath", filePath);
    }
    async removePath(filePath) {
      return this.callMethod("removePath", filePath);
    }
    callMethod(methodName, ...params) {
      return this.call({
        className: "fs",
        winId: globalThis.__WIN_ID,
        tarId: globalThis.__WIN_ID,
        methodName,
        params
      });
    }
  };

  // Dialog.ts
  var Dialog = class extends Eventer {
    async openPathDialog(config) {
      return this.callMethod("openPathDialog", config);
    }
    callMethod(methodName, ...params) {
      return this.call({
        className: "dialog",
        winId: globalThis.__WIN_ID,
        tarId: globalThis.__WIN_ID,
        methodName,
        params
      });
    }
  };

  // Clipboard.ts
  var Clipboard = class extends Eventer {
    readText() {
      return this.callMethod("readText");
    }
    writeText(text) {
      return this.callMethod("writeText", text);
    }
    callMethod(methodName, ...params) {
      return this.call({
        className: "clipboard",
        winId: globalThis.__WIN_ID,
        tarId: globalThis.__WIN_ID,
        methodName,
        params
      });
    }
  };

  // Net.ts
  var Net = class extends Eventer {
    async getAddress() {
      return this.callMethod("getAddress");
    }
    callMethod(methodName, ...params) {
      return this.call({
        className: "net",
        winId: globalThis.__WIN_ID,
        tarId: globalThis.__WIN_ID,
        methodName,
        params
      });
    }
  };

  // Horse.ts
  var Horse = class extends Eventer {
    win;
    fs;
    dialog;
    clipboard;
    net;
    webview;
    constructor() {
      super();
      this.webview = window.chrome.webview;
      this.win = new Win();
      this.fs = new Fs();
      this.dialog = new Dialog();
      this.clipboard = new Clipboard();
      this.net = new Net();
      this.listenMsg();
    }
    getConfig() {
      return this.callMethod("getConfig");
    }
    async createWindow(config) {
      let obj = await this.callMethod("createWindow", config);
      return new WinProx(obj.id, this.win);
    }
    listenMsg() {
      this.webview.addEventListener("message", (e) => {
        let clsName = e.data.className;
        delete e.data.className;
        let evtName = e.data.eventName;
        delete e.data.eventName;
        if (clsName === "clipboard") {
          this.clipboard.emit(evtName, e.data);
        } else if (clsName === "dialog") {
          this.dialog.emit(evtName, e.data);
        } else if (clsName === "fs") {
          this.fs.emit(evtName, e.data);
        } else if (clsName === "horse") {
          this.emit(evtName, e.data);
        } else if (clsName === "net") {
          this.net.emit(evtName, e.data);
        } else if (clsName === "win") {
          this.win.emit(evtName, e.data);
        }
      });
      this.webview.addEventListener("sharedbufferreceived", (e) => {
        const buffer = e.getBuffer();
        let clsName = e.additionalData.className;
        delete e.additionalData.className;
        let evtName = e.additionalData.eventName;
        delete e.additionalData.eventName;
        if (clsName === "fs") {
          this.fs.emit(evtName, {
            buffer,
            ...e.additionalData,
            release: () => {
              window.chrome.webview.releaseBuffer(buffer);
            }
          });
        }
      });
    }
    callMethod(methodName, ...params) {
      return this.call({
        className: "horse",
        winId: globalThis.__WIN_ID,
        tarId: globalThis.__WIN_ID,
        methodName,
        params
      });
    }
  };
  globalThis.horse = new Horse();
})();
