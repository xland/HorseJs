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