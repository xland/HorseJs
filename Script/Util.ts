class Util {
  postMsg(json: any) {
    window.chrome.webview.postMessage(json);
  }
  //生成一个12位的随机数字
  randomNum(len: number = 9): number {
    return Math.floor(Math.pow(10, len) * Math.random());
  }
}
export let util = new Util();
