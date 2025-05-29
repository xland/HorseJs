import { Window } from "./Window";
import { Fs } from "./Fs";
import { Eventer } from "./Eventer";
class Horse extends Eventer {
  window: Window;
  fs: Fs;
  webview;
  constructor() {
    super();
    this.webview = window.chrome.webview;
    this.window = new Window(); // 初始化 window 实例
    this.fs = new Fs();
    this.listenMsg();
  }
  getConfig() {
    return this.callMethod("getConfig");
  }
  private listenMsg() {
    this.webview.addEventListener("message", (e) => {
      if (e.data.className === "horse") {
        this.emit(e.data.eventName, e.data);
      } else if (e.data.className === "window") {
        this.window.emit(e.data.eventName, e.data);
      } else if (e.data.className === "fs") {
        this.fs.emit(e.data.eventName, e.data);
      }
    });
    this.webview.addEventListener("sharedbufferreceived", (e) => {
      const buffer = e.getBuffer();
      const metadata = e.additionalData;
      chunks.push({ offset: metadata.offset, data: new Uint8Array(buffer) });

      // 检查是否收到所有块
      if (metadata.offset + buffer.byteLength >= metadata.totalSize) {
        // 按 offset 排序并拼接
        chunks.sort((a, b) => a.offset - b.offset);
        const totalSize = metadata.totalSize;
        const fullData = new Uint8Array(totalSize);
        chunks.forEach((chunk) => {
          fullData.set(chunk.data, chunk.offset);
        });
        console.log("Full file received:", fullData);
        // 示例：转换为 Blob 或其他格式
        const blob = new Blob([fullData], { type: "application/octet-stream" });
        // 释放所有缓冲区
        chunks.forEach((chunk) => window.chrome.webview.releaseBuffer(chunk.data.buffer));
        chunks = [];
      } else {
        window.chrome.webview.releaseBuffer(buffer);
      }
    });
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "horse",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
globalThis.horse = new Horse();
