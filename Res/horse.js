(() => {
  // Util.ts
  var Util = class {
    postMsg(json) {
      window.chrome.webview.postMessage(json);
    }
    randomNum() {
      const min = 2e3;
      const max = 999999999;
      return Math.floor(Math.random() * (max - min + 1)) + min;
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
        obj.eventName = `${util.randomNum()}`;
        this.once(obj.eventName, (result) => {
          resolve(result);
        });
        window.chrome.webview.postMessage(obj);
      });
    }
  };

  // Process.ts
  var Process = class extends Eventer {
    async exec(path) {
      return this.callMethod("exec", path);
    }
    callMethod(methodName, ...params) {
      let obj = window.self === window.top ? this : window.top.horse.process;
      return obj.call({
        className: "process",
        winId: globalThis.__WIN_ID,
        methodName,
        params
      });
    }
  };

  // Tray.ts
  var Tray = class extends Eventer {
    async create(config) {
      config.__id = util.randomNum();
      this.on(config.__id, (data) => {
        let type = data.type;
        delete data.type;
        config[type]();
      });
      config.menu.forEach((item) => {
        item.__id = util.randomNum();
        this.on(item.__id, item.click);
      });
      return this.callMethod("create", config);
    }
    callMethod(methodName, ...params) {
      let obj = window.self === window.top ? this : window.top.horse.tray;
      return obj.call({
        className: "tray",
        winId: globalThis.__WIN_ID,
        methodName,
        params
      });
    }
  };

  // Notify.ts
  var Notify = class extends Eventer {
    async show(appName, title, content) {
      return this.callMethod("show", appName, title, content);
    }
    callMethod(methodName, ...params) {
      let obj = window.self === window.top ? this : window.top.notify;
      return obj.call({
        className: "notify",
        winId: globalThis.__WIN_ID,
        methodName,
        params
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
      let obj = window.self === window.top ? this : window.top.horse.win;
      return obj.call({
        className: "win",
        winId: globalThis.__WIN_ID,
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
      let obj = window.self === window.top ? this.parent : window.top.horse.win;
      return obj.call({
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
    async createFile(filePath) {
      return this.callMethod("createFile", filePath);
    }
    async createDir(dirPath) {
      return this.callMethod("createDir", dirPath);
    }
    async ensurePath(filePath) {
      return this.callMethod("ensurePath", filePath);
    }
    async listDir(dirPath) {
      return this.callMethod("listDir", dirPath);
    }
    async movePath(srcPath, dstPath) {
      return this.callMethod("movePath", srcPath, dstPath);
    }
    callMethod(methodName, ...params) {
      let obj = window.self === window.top ? this : window.top.horse.fs;
      return obj.call({
        className: "fs",
        winId: globalThis.__WIN_ID,
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
    async savePathDialog(config) {
      return this.callMethod("savePathDialog", config);
    }
    async msgBox(config) {
      return this.callMethod("msgBox", config);
    }
    callMethod(methodName, ...params) {
      let obj = window.self === window.top ? this : window.top.horse.dialog;
      return obj.call({
        className: "dialog",
        winId: globalThis.__WIN_ID,
        methodName,
        params
      });
    }
  };

  // Clipboard.ts
  var Clipboard = class extends Eventer {
    getDataType() {
      return this.callMethod("getDataType");
    }
    readText() {
      return this.callMethod("readText");
    }
    writeText(text) {
      return this.callMethod("writeText", text);
    }
    readHtml() {
      return this.callMethod("readHtml");
    }
    writeHtml(html) {
      return this.callMethod("writeHtml", html);
    }
    readRtf() {
      return this.callMethod("readRtf");
    }
    writeRtf(rtf) {
      return this.callMethod("writeRtf", rtf);
    }
    readImg() {
      return this.callMethod("readImg");
    }
    writeImg(rtf) {
      return this.callMethod("writeImg", rtf);
    }
    getFile() {
      return this.callMethod("getFile");
    }
    addFile(...paths) {
      return this.callMethod("addFile", ...paths);
    }
    clear() {
      return this.callMethod("clear");
    }
    callMethod(methodName, ...params) {
      let obj = window.self === window.top ? this : window.top.horse.clipboard;
      return obj.call({
        className: "clipboard",
        winId: globalThis.__WIN_ID,
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
      let obj = window.self === window.top ? this : window.top.horse.net;
      return obj.call({
        className: "net",
        winId: globalThis.__WIN_ID,
        methodName,
        params
      });
    }
  };

  // Os.ts
  var Os = class extends Eventer {
    async getVersion() {
      return this.callMethod("getVersion");
    }
    callMethod(methodName, ...params) {
      let obj = window.self === window.top ? this : window.top.horse.os;
      return obj.call({
        className: "os",
        winId: globalThis.__WIN_ID,
        methodName,
        params
      });
    }
  };

  // Screen.ts
  var Screen = class extends Eventer {
    async getAll() {
      return this.callMethod("getAll");
    }
    callMethod(methodName, ...params) {
      let obj = window.self === window.top ? this : window.top.horse.screen;
      return obj.call({
        className: "screen",
        winId: globalThis.__WIN_ID,
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
    notify;
    lib;
    net;
    os;
    screen;
    tray;
    process;
    webview;
    constructor() {
      super();
      this.webview = window.chrome.webview;
      this.win = new Win();
      this.fs = new Fs();
      this.dialog = new Dialog();
      this.clipboard = new Clipboard();
      this.net = new Net();
      this.notify = new Notify();
      this.os = new Os();
      this.screen = new Screen();
      this.tray = new Tray();
      this.process = new Process();
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
      if (window.self !== window.top) return;
      this.webview.addEventListener("message", (e) => {
        let clsName = e.data.className;
        delete e.data.className;
        let evtName = e.data.eventName;
        delete e.data.eventName;
        if (clsName !== "horse") {
          this[clsName].emit(evtName, e.data);
        } else {
          this.emit(evtName, e.data);
        }
      });
      this.webview.addEventListener("sharedbufferreceived", (e) => {
        const buffer = e.getBuffer();
        let clsName = e.additionalData.className;
        delete e.additionalData.className;
        let evtName = e.additionalData.eventName;
        delete e.additionalData.eventName;
        if (clsName !== "horse") {
          this[clsName].emit(evtName, {
            buffer,
            ...e.additionalData,
            release: () => {
              window.chrome.webview.releaseBuffer(buffer);
            }
          });
        } else {
          this.emit(evtName, {
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
      let obj = window.self === window.top ? this : window.top.horse;
      return obj.call({
        className: "horse",
        winId: globalThis.__WIN_ID,
        methodName,
        params
      });
    }
  };
  globalThis.horse = new Horse();
})();
