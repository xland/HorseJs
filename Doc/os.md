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