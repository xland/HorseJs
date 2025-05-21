import { Eventer } from "./Eventer";
import { FsMethodId, ClassId, FsEventId } from "./EnumId";

export class Fs extends Eventer {
  addResToExe(dirPath: string, exePath: string) {
    return this.call(ClassId.Fs, FsMethodId.addResToExe, dirPath, exePath);
  }
}
