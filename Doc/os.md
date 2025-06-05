- 获得系统版本号

```js
let data = await horse.os.getVersion();
console.log(data);
//{"buildNumber":19044,"majorVersion":10,"minorVersion":0,"ok":true,"versionName":"win10"}
```