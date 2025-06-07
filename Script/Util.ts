class Util {
  postMsg(json: any) {
    window.chrome.webview.postMessage(json);
  }
  randomNum(): number {
    const min = 2000; //避开WM_USER及相关的自定义消息
    const max = 999999999; //最多9位，避免超过C++ int的最大值
    return Math.floor(Math.random() * (max - min + 1)) + min;
  }
}
export let util = new Util();
