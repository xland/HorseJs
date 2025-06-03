## 说明

HorseJs 框架会为每个窗口的页面注入一个JavaScript 对象：`horse`

开发者与 HorseJs 交互的大部分操作都是通过这个对象（及其子对象）完成的。

horse对象（及其子对象）公开的每个方法都是异步的。

## horse 的方法

- 获取应用程序配置信息 ( config.json 中的信息)
```js
let config = await horse.getConfig();
console.log(config);  //config是一个json对象
```

- 创建一个新窗口

> 新窗口创建成功后，将得到一个窗口代理对象（是 [Win](./win.md) 的子类型），<br />
> 你可以使用这个对象来操作你刚刚创建的窗口（包括监听这个窗口上的事件）。

```js
let proxyWin = await horse.createWindow({
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


