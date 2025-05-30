此项目前正在积极开发中...

## HorseJs

- 使用HTML/JS/CSS创建更快速、更稳定的桌面应用。
- 使用更简单：开发者只在渲染进程内工作。
- 体积更小，基于WebView2，仅一个可执行文件（小于1M）

## 下载

[Release](https://github.com/xland/HorseJs/releases) （小于1M）

## 目录结构

```shell
├─ HorseJs.exe          # 你可以随意修改此文件的文件名
├─ UI                   # 此目录放置配置文件和HTML/CSS/JS文件，必须叫这个名字
   ├─ config.json       # 应用程序配置文件，必须叫这个名字
   ├─ css/              # 网页样式目录，只要在UI目录下即可，叫什么名字都可以
   ├─ js/               # 网页脚本目录，只要在UI目录下即可，叫什么名字都可以
```

## config.json

- appId
> - 应用程序的Id。必须是英文，不能包含特殊符号
- quitWhenNoWindow
> - 类型：布尔，默认值：`true`
> 当所有窗口关闭时，退出应用程序。
- window.resizable
> - 类型：布尔，默认值：`true`
> - 是否允许调整窗口的大小。
 - window.size
> - 默认值：`{w:1000,h:800}`
> - 值为：`"maximize"` 时，窗口将在当前屏幕全屏显示，但不覆盖任务栏。
> - 值为：`"fullScreen"` 时，窗口将在当前屏幕全屏显示，覆盖任务栏。
> - 值为：`"fullDesktop"` 时，窗口将覆盖所有屏幕并全屏显示，覆盖任务栏。
> - 值为：`{w:1000,h:800}`时，窗口将按指定大小显示。
 - window.miniSize
> - 默认值：`{w:250,h:200}` ，设置窗口的最小大小。
 - window.maxSize
> - 示例值：`{w:2000,h:1600}` ，设置窗口的最大大小。
 - window.position
> - 默认值：`"centerScreen"`
> - 值为：`"centerScreen"` 时,窗口将出现在当前屏幕正中央。
> - 值为：`{x:100,y:100}` 时，窗口将在指定位置显示 。
 - window.visible
> - 类型：布尔，默认值：`true`。
> - 值为：`true` 时，创建窗口即显示窗口，无论窗口是否加载完HTML。
> - 值为：`false` 时，窗口创建时不显示，你可以在加载完HTML之后，再手动控制显示窗口。
 - window.frame
> - 类型：布尔，默认值：`true`。
> - 窗口是否拥有边框，如果为true，则窗口具备标题栏和边框，是一个普通窗口。
 - window.shadow
> - 类型：布尔，默认值：`true`。
> - 窗口是否拥有有阴影，window.frame 为 true 时，此属性无效。
 - window.minimizable
> - 类型：布尔，默认值：`true`。是否允许窗口最小化
 - window.maximizable 
> - 类型：布尔，默认值：`true`。是否允许窗口最大化
 - window.alwaysOnTop
> - 类型：布尔，默认值：`false`。是否置顶窗口
 - window.skipTaskbar
> - 类型：布尔，默认值：`true`。是否置在任务栏中显示
 - window.transparent 
> - 类型：布尔，默认值：`false`。
> - 窗口是否背景透明，window.frame 为 true 时，此属性无效。
- window.webview.url
> - 类型：字符串，默认值：`index.html`
> - 实际会被转型为：`"https://{appId}/index.html"`
 - window.webview.allowWindowOpen
> - 类型：布尔，默认值：`true`。
> - 是否允许在页面中打开新窗口。
 - window.title
> - 类型：字符串，默认值：`Window - HorseJs`。
> - 窗口标题，HTML的title将覆盖此设置。

## horse 的方法

- 获取应用程序配置信息 config.json中的信息
```js
let config = await horse.getConfig();
console.log(config);  //config是一个json对象
```
## horse 的事件



## horse.window 的方法

- 最大化
```js
// horse是框架自动为页面注册的对象。
// horse.window是当前页面所在窗口的对象。
// maximize方法用于控制当前页面所在窗口最大化。
await horse.window.maximize();
```
- 最小化
```js
await horse.window.minimize();
```
- 还原
```js
await horse.window.restore();
```
- 显示
```js
await horse.window.show();
```
- 隐藏
```js
await horse.window.hide();
```
- 改变大小
```js
await horse.window.resize(1200, 600);
```
- 关闭窗口
```js
await horse.window.close();
```
- 销毁窗口
```js
await horse.window.destroy();
```
- 让窗口闪烁
```
horse.window.flash(true);
```
- 开始通过拖拽改变窗口位置
```js
//此方法常用于设置窗口的自定义标题栏
$(".titleBar").addEventListener("mousedown",async ()=>{
    horse.window.startDrag();
})
```
- 创建一个新窗口
```js
horse.window.openWindow({
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
    "position": {
        "x":200,
        "y":200
    },
    "page": {
        "areDefaultScriptDialogsEnabled": true,
        "isScriptEnabled": true,
        "isWebMessageEnabled": true,
        "url": "index.html"
    }
});
```
- 注册事件
```js
//注册窗口位置或大小改变事件
horse.window.addEventListener("sizePosChanged",(x,y,w,h)=>{
    console.log(`x:${x},y:${y},w:${w},h:${h}`);
})
```
- 取消注册事件
```js
//取消 注册的窗口关闭事件
await horse.window.removeEventListener("closing");
```



## horse.window 的事件

- 窗口位置或大小改变事件
```js
horse.window.addEventListener("sizePosChanged",(x,y,w,h)=>{
    console.log(`x:${x},y:${y},w:${w},h:${h}`);
})
```

- 窗口关闭事件
```js
// 注册窗口关闭事件，此事件注册后，窗口将不允许关闭
// 想要关闭窗口，必须调用：horse.window.destroy()
horse.window.addEventListener("closing", async ()=>{
    alert("现在你只能用销毁窗口关闭此窗口了");
});
```

## horse.fs 的方法

- 读取文件全部内容
```
let data = await horse.fs.readFile("D:\\index.html");
const uint8Array = new Uint8Array(data.buffer); // 字节视图
const decoder = new TextDecoder('utf-8'); // UTF-8 解码器
const text = decoder.decode(uint8Array); // 转换为字符串
console.log('Decoded string:', text);
data.release(); //释放资源
```
- 读取文件的一部分
```js
//从第10个字节开始，读取100个字节
let data = await horse.fs.readFileChunk("D:\\index.html",10,100);
const uint8Array = new Uint8Array(data.buffer);
const decoder = new TextDecoder('utf-8');
const text = decoder.decode(uint8Array);
console.log('Decoded string:', text);
data.release();
```
- 路径是否存在
```js
let data = await horse.fs.exists("D:\\index.html");
console.log(data);
// 输出：{isDir: false, isExists: true}
```
- 获取文件信息
```js
let data = await horse.fs.getFileInfo("D:\\index.html");
console.log(data);
// 输出：{createTime:1747615428326,fileSize:9190,isHidden:false,isReadOnly:false,isSystem:false,lastAccessTime:1748565302829,lastWriteTime:1748565107113}
```
## horse.fs 的事件

## FAQ

- 为什么不能在一个窗口的页面内操作另一个窗口？
```
方法执行完是有返回值的，返回值是通过消息发送给目标窗口的。
如果要实现此需求，发送任何一个消息时，都要遍历窗口列表。
况且还要考虑在A窗口内，注册B窗口的事件。
```


## 下载

[Release](https://github.com/xland/HorseJs/releases) （100kb）

## 赞助
<table>
  <tr>
    <td align="center">
      <img src="./Doc/alipay.jpg" width="160" height="160">
      <p>支付宝赞助</p>
    </td>
    <td align="center">
      <img src="./Doc/wechat.png" width="160" height="160">
      <p>微信赞助</p>
    </td>
    <td align="center">
      <img src="./Doc/author.jpg" width="160" height="160">
      <p>作者微信</p>
    </td>
    <td align="center">
      <img src="./Doc/gongzhonghao.jpg" width="160" height="160">
      <p>公众号：桌面软件</p>
    </td>
  </tr>
</table>