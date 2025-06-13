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

- 获得应用的版本号
> 返回结果中exeVer是应用程序的版本号（exe文件的版本信息）<br />
> horseVer是当前应用所使用的HorseJs框架的版本号
```js
let data = await horse.getVersion();
console.log(data);
// 输出：{"exeVer":[0,0,10,0],"horseVer":[0,0,10,0],"ok":true}
```

- 关闭所有窗口，并退出应用
> 如果窗口注册了closing事件，那么应用不会退出，而是返回一个错误
```js
let data = await horse.quit();
console.log(JSON.stringify(data));
```

- 强制退出应用
```js
horse.exit(1);
```

- 获取Exe可执行文件的版本号
```js
let data = await horse.getExeVer();
console.log(JSON.stringify(data));
//{"build":0,"major":0,"minor":0,"ok":true,"patch":10}
```

- 获取HorseJs框架的版本号
```js
let data = await horse.getHorseVer();
console.log(JSON.stringify(data));
//{"build":0,"major":0,"minor":0,"ok":true,"patch":10}
```

- 启动当前应用的一个新实例
```js
let data = await horse.relaunch();
console.log(JSON.stringify(data));
```

- 监听新窗口创建事件
```js
let func = (proxy) => {
    console.log(proxy.id);
    proxy.minimize();
};
await horse.on("newWin", func);
```

- 取消监听新窗口创建事件
```js
await horse.off("newWin",func);
```
- 监听窗口关闭事件
```js
await horse.on("winClose", (data) => {
    console.log(data);
});
```
- 取消监听窗口关闭事件
```js
await horse.off("winClose",func);
```
- 监听应用第二个实例启动事件
> 需配合设置config.json中的 <br/>
>  "instanceWatch": true, <br/>
>  "instanceLock": true, <br/>
```js
await horse.on("secondIns", () => {
  console.log("secondIns");
});
```

- 取消监听应用第二个实例启动事件
```js
await horse.off("secondIns",func);
```

- 允许启动第二个实例
> 需配合设置config.json中的 <br/>
>  "instanceWatch": true, <br/>
>  "instanceLock": true, <br/>
```js
let obj = await horse.enableSecondIns();
console.log(obj);
```

- 禁止启动第二个实例
> 需配合设置config.json中的 <br/>
>  "instanceWatch": true, <br/>
>  "instanceLock": true, <br/>
```js
let obj = await horse.disableSecondIns();
console.log(obj);
```