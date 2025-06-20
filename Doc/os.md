- 获得系统版本号

```js
let data = await horse.os.getVersion();
console.log(data);
//{"buildNumber":19044,"majorVersion":10,"minorVersion":0,"ok":true,"versionName":"win10"}
```
- 创建一个快捷方式

```js
let data = await horse.os.createShortcut("C:\\Windows\\System32\\cmd.exe", 
	"C:\\Users\\liuxiaolun\\Desktop\\cmd.lnk", 
	"命令行", 
	"C:\\Windows\\System32\\");
console.log(data);
```
- 获取CPU ID

```js
let data = await horse.os.getCPUID();
console.log(JSON.stringify(data));
// {"data":"000906edbfebfbff","ok":true}
```
- 获取磁盘串号

```js
let data = await horse.os.getDiskSerialNumber();
console.log(JSON.stringify(data));
// {"data":"92adfb09","ok":true}
```
- 获取当前用户使用的语言

```js
let data = await horse.os.getUserLang();
console.log(JSON.stringify(data));
// {"data":"zh-CN","ok":true}
```
- 获取系统语言

```js
let data = await horse.os.getOsLang();
console.log(JSON.stringify(data));
//  {"data":"zh-CN","ok":true}
```
- 获取当前系统使用的颜色

```js
let data = await horse.os.getOsColor();
console.log(JSON.stringify(data));
//  {"bgColor":"#0063B1","borderColor":"#B4B4B4","btnColor":"#BFCDDB","captionColor":"#000000","ok":true,"textColor":"#000000","winColor":"#FFFFFF"}
```

- 打开一个文件路径：显示此文件所在的文件夹，并选中文件。

```js
await horse.os.showItemInFolder("C:\\Windows\\System32\\cmd.exe");
```

- 使用默认应用打开一个文件路径。

```js
await horse.os.openFile("D:\\doc\\123.docx");
```

- 阻止系统睡眠、休眠。

```js
await horse.os.preventSleep();
```

- 取消阻止系统睡眠、休眠。

```js
await horse.os.stopPreventSleep();
```

- 监听系统锁屏事件。

```js
await horse.os.on("osLock", (data) => {
	console.log(JSON.stringify(data));
});
// {"ok":true,"type":"lock"}
// {"ok":true,"type":"unlock"}
```

- 取消监听系统锁屏事件。

```js
$("#btnOffOsLock").addEventListener("click", async () => {
    await horse.os.off("osLock");
});
```