import { util } from "./Util";
import { Eventer } from "./Eventer";
export class Fs extends Eventer {
  async readFile(filePath: string) {
    return this.callMethod("readFile", filePath);
  }
  async readFileChunk(filePath: string, startPos: number, chunkSize: number) {
    return this.callMethod("readFileChunk", filePath, startPos, chunkSize);
  }
  private callMethod(methodName: string, ...params: any[]) {
    return this.call({
      className: "fs",
      winId: globalThis.__WIN_ID,
      methodName,
      params,
    });
  }
}
