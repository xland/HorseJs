interface Result<T = void> {
  ok: boolean;
  err?: string;
  data?: T;
}

interface Win {
  startDrag: () => {};
  on(evtName: string, cb: (result: { state: string }) => void): Result;
  minimize(): Result;
  restore(): Result;
  /**
   *
   */
  maximize(): Result;
  /**
   * 关闭窗口
   * @returns
   */
  close(): Result;
}
interface Db {
  /**
   *
   * @param dbPath
   * @param inDataPath
   */
  open(
    dbPath: string,
    inDataPath?: boolean
  ): Result<{
    /**
     * 是否第一次创建数据库
     */
    isDbFirstCreated: boolean;
    data: string;
  }>;
}
interface Horse {
  win: Win;
  db: Db;
}
declare var horse: Horse;
