import { util } from "./Util";
import { Eventer } from "./Eventer";
export class Fs extends Eventer {
  readFile(filePath: string) {
    return this.exec("readFile", filePath);
  }
  readFileChunk(filePath: string, startPos: number, chunkSize: number) {
    return this.exec("readFileChunk", filePath, startPos, chunkSize);
  }
  exists(filePath: string) {
    return this.exec("exists", filePath);
  }
  getFileInfo(filePath: string) {
    return this.exec("getFileInfo", filePath);
  }
  writeFile(filePath: string, content: string) {
    return this.exec("writeFile", filePath, content);
  }
  writeFileChunk(filePath: string, content: string, startPos: number) {
    return this.exec("writeFileChunk", filePath, content, startPos);
  }
  delPath(filePath: string) {
    return this.exec("delPath", filePath);
  }
  removePath(filePath: string) {
    return this.exec("removePath", filePath);
  }
  createFile(filePath: string) {
    return this.exec("createFile", filePath);
  }
  createDir(dirPath: string) {
    return this.exec("createDir", dirPath);
  }
  ensurePath(filePath: string) {
    return this.exec("ensurePath", filePath);
  }
  listDir(dirPath: string) {
    return this.exec("listDir", dirPath);
  }
  movePath(srcPath: string, dstPath: string) {
    return this.exec("movePath", srcPath, dstPath);
  }
  copyPath(srcPath: string, dstPath: string) {
    return this.exec("copyPath", srcPath, dstPath);
  }
  renamePath(srcPath: string, dstPath: string) {
    return this.exec("renamePath", srcPath, dstPath);
  }
  getPath(type: string) {
    return this.exec("getPath", type);
  }
  watch(path: string, cb: () => {}) {
    let id = `${util.randomNum()}`;
    this.listen(id, cb);
    return this.exec("watch", path, id);
  }
  stopWatch(id: string) {
    this.unlisten(id);
    return this.exec("stopWatch", id);
  }
  createShortcut(srcPath: string, dstSrc: string, des: string, workDir: string) {
    return this.exec("createShortcut", srcPath, dstSrc, des, workDir);
  }
  openFile(filePath: string) {
    return this.exec("openFile", filePath);
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
