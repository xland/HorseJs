import { util } from "./Util";
import { Eventer } from "./Eventer";
export class Fs extends Eventer {
  async readFile(filePath: string) {
    return this.callMethod("readFile", filePath);
  }
  async readFileChunk(filePath: string, startPos: number, chunkSize: number) {
    return this.callMethod("readFileChunk", filePath, startPos, chunkSize);
  }
  async exists(filePath: string) {
    return this.callMethod("exists", filePath);
  }
  async getFileInfo(filePath: string) {
    return this.callMethod("getFileInfo", filePath);
  }
  async writeFile(filePath: string, content: string) {
    return this.callMethod("writeFile", filePath, content);
  }
  async writeFileChunk(filePath: string, content: string, startPos: number) {
    return this.callMethod("writeFileChunk", filePath, content, startPos);
  }
  async delPath(filePath: string) {
    return this.callMethod("delPath", filePath);
  }
  async removePath(filePath: string) {
    return this.callMethod("removePath", filePath);
  }
  async createFile(filePath: string) {
    return this.callMethod("createFile", filePath);
  }
  async createDir(dirPath: string) {
    return this.callMethod("createDir", dirPath);
  }
  async ensurePath(filePath: string) {
    return this.callMethod("ensurePath", filePath);
  }
  async listDir(dirPath: string) {
    return this.callMethod("listDir", dirPath);
  }
  async movePath(srcPath: string, dstPath: string) {
    return this.callMethod("movePath", srcPath, dstPath);
  }
  async copyPath(srcPath: string, dstPath: string) {
    return this.callMethod("copyPath", srcPath, dstPath);
  }
  private callMethod(methodName: string, ...params: any[]) {
    let obj = window.self === window.top ? this : window.top.horse.fs;
    return obj.call({
      className: "fs",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
