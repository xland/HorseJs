## 屏幕

- 获取所有屏幕的信息
> left1 top1 bottom1 right1 是屏幕矩形区域坐标 <br />
> left2 top2 bottom2 right2 是屏幕工作区域坐标
```js
let data = await horse.screen.getAll();
console.log(data);
//输出 {"data":[{
// 	  "bottom1":2160,
// 	  "bottom2":2114,
// 	  "device":"\\\\.\\DISPLAY1",
// 	  "isMain":true,
// 	  "left1":0,
// 	  "left2":0,
// 	  "right1":3840,
// 	  "right2":3840,
// 	  "scaleFactor":150,
// 	  "top1":0,
// 	  "top2":0
// }],"ok":true}
```

- 获取桌面矩形

```js
let data = await horse.screen.getDesktop();
console.log(data);
//输出 {h: 2160, w: 3840, x: 0, y: 0,ok: true}
```

- 获取屏幕上某一点的颜色
> 如果不输入坐标，则返回鼠标所在位置的颜色
```js
let data = await horse.screen.getColor(100, 100);
console.log(data);
> {data: '#CCD5F0', ok: true}
```