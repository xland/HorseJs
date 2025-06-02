## horse 的方法

- 获取应用程序配置信息 config.json中的信息
```js
let config = await horse.getConfig();
console.log(config);  //config是一个json对象
```

- 创建一个新窗口
```js
let proxyWin = await horse.openWindow({
    "resizable": true,
    "maximizable": false,
    "minimizable":false,
    "alwaysOnTop":false,
    "skipTaskbar":false,
    "visible": true,
    "frame": false,
    "shadow": true,
    "title": "窗口标题！！！",
    "size": {
        "w": 1000,
        "h": 800
    },
    "minSize": {
        "w": 250,
        "h": 200
    },
    "position": "centerScreen",
    "page": {
        "areDefaultScriptDialogsEnabled": true,
        "isScriptEnabled": true,
        "isWebMessageEnabled": true,
        "url": "index.html"
    }
});
//监听新开窗口的事件
await proxyWin.addEventListener("sizePosChanged", (data) => {
  console.log(`x:${data.x},y:${data.y},w:${data.w},h:${data.h}`);
});
```
## horse 的事件


