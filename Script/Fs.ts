import { util } from "./Util";
import { Eventer } from "./Eventer";
export class Fs extends Eventer {
  async readFile(filePath: string) {
    return this.exec("readFile", filePath);
  }
  async readFileChunk(filePath: string, startPos: number, chunkSize: number) {
    return this.exec("readFileChunk", filePath, startPos, chunkSize);
  }
  async exists(filePath: string) {
    return this.exec("exists", filePath);
  }
  async getFileInfo(filePath: string) {
    return this.exec("getFileInfo", filePath);
  }
  async writeFile(filePath: string, content: string) {
    return this.exec("writeFile", filePath, content);
  }
  async writeFileChunk(filePath: string, content: string, startPos: number) {
    return this.exec("writeFileChunk", filePath, content, startPos);
  }
  async delPath(filePath: string) {
    return this.exec("delPath", filePath);
  }
  async removePath(filePath: string) {
    return this.exec("removePath", filePath);
  }
  async createFile(filePath: string) {
    return this.exec("createFile", filePath);
  }
  async createDir(dirPath: string) {
    return this.exec("createDir", dirPath);
  }
  async ensurePath(filePath: string) {
    return this.exec("ensurePath", filePath);
  }
  async listDir(dirPath: string) {
    return this.exec("listDir", dirPath);
  }
  async movePath(srcPath: string, dstPath: string) {
    return this.exec("movePath", srcPath, dstPath);
  }
  async copyPath(srcPath: string, dstPath: string) {
    return this.exec("copyPath", srcPath, dstPath);
  }
  async renamePath(srcPath: string, dstPath: string) {
    return this.exec("renamePath", srcPath, dstPath);
  }
  getPath(type: string) {
    return this.exec("getPath", type);
  }
  async watch(path: string, cb: () => {}) {
    let id = `${util.randomNum()}`;
    this.listen(id, cb);
    return this.exec("watch", path, id);
  }
  async stopWatch(id: string) {
    this.unlisten(id);
    return this.exec("stopWatch", id);
  }
  createShortcut(srcPath: string, dstSrc: string, des: string, workDir: string) {
    return this.exec("createShortcut", srcPath, dstSrc, des, workDir);
  }
  private exec(methodName: string, ...params: any[]) {
    return this.call({
      className: "fs",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
