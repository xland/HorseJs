class Util {
  postMsg(json: any) {
    window.chrome.webview.postMessage(json);
  }
  randomNum(): number {
    return Math.floor(Math.random() * 1000000000); //最多9位，避免超过C++ int的最大值
  }
}
export let util = new Util();
