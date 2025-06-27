import { util } from "./Util";
import { Eventer } from "./Eventer";
export class Fs extends Eventer {
  readFile(filePath: string) {
    return this.execute("readFile", filePath);
  }
  readFileChunk(filePath: string, startPos: number, chunkSize: number) {
    return this.execute("readFileChunk", filePath, startPos, chunkSize);
  }
  exists(filePath: string) {
    return this.execute("exists", filePath);
  }
  getFileInfo(filePath: string) {
    return this.execute("getFileInfo", filePath);
  }
  writeFile(filePath: string, content: string) {
    return this.execute("writeFile", filePath, content);
  }
  writeFileChunk(filePath: string, content: string, startPos: number) {
    return this.execute("writeFileChunk", filePath, content, startPos);
  }
  delPath(filePath: string) {
    return this.execute("delPath", filePath);
  }
  removePath(filePath: string) {
    return this.execute("removePath", filePath);
  }
  createFile(filePath: string) {
    return this.execute("createFile", filePath);
  }
  createDir(dirPath: string) {
    return this.execute("createDir", dirPath);
  }
  ensurePath(filePath: string) {
    return this.execute("ensurePath", filePath);
  }
  listDir(dirPath: string) {
    return this.execute("listDir", dirPath);
  }
  movePath(srcPath: string, dstPath: string) {
    return this.execute("movePath", srcPath, dstPath);
  }
  copyPath(srcPath: string, dstPath: string) {
    return this.execute("copyPath", srcPath, dstPath);
  }
  renamePath(srcPath: string, dstPath: string) {
    return this.execute("renamePath", srcPath, dstPath);
  }
  getPath(type: string) {
    return this.execute("getPath", type);
  }
  watch(path: string, cb: () => {}) {
    let id = `${util.randomNum()}`;
    this.listen(id, cb);
    return this.execute("watch", path, id);
  }
  stopWatch(id: string) {
    this.unlisten(id);
    return this.execute("stopWatch", id);
  }
  createShortcut(srcPath: string, dstSrc: string, des: string, workDir: string) {
    return this.execute("createShortcut", srcPath, dstSrc, des, workDir);
  }
  openFile(filePath: string) {
    return this.execute("openFile", filePath);
  }
  private execute(methodName: string, ...params: any[]) {
    return this.call({
      className: "fs",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
