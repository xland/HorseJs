## horse.win 的方法

- 最大化
```js
// horse.win是当前页面所在窗口的对象。
// maximize方法用于控制当前页面所在窗口最大化。
await horse.win.maximize();
```
- 最小化
```js
await horse.win.minimize();
```
- 还原
```js
await horse.win.restore();
```
- 显示
```js
await horse.win.show();
```
- 隐藏
```js
await horse.win.hide();
```
- 改变大小
```js
await horse.win.resize(1200, 600);
```
- 关闭窗口
```js
await horse.win.close();
```
- 销毁窗口
```js
await horse.win.destroy();
```
- 让窗口闪烁
```
horse.win.flash(true);
```
- 开始通过拖拽改变窗口位置
```js
//此方法常用于设置窗口的自定义标题栏
$(".titleBar").addEventListener("mousedown",async ()=>{
    horse.win.startDrag();
})
```

- 注册事件
```js
//注册窗口位置或大小改变事件
horse.win.addEventListener("sizePosChanged",(x,y,w,h)=>{
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
> 注册窗口关闭事件，此事件注册后，窗口将不允许关闭 <br />
> 想要关闭窗口，必须调用：horse.window.destroy()
```js
horse.window.addEventListener("closing", async ()=>{
    alert("现在你只能用销毁窗口关闭此窗口了");
});
```
