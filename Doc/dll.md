用于加载第三方库

- 加载第三方库
> 返回值的id，用于后续操作
```js
let data = await horse.lib.load("./TestDll.dll");
console.log(data)
// {ok: true,id:1}
```
- 释放第三方库
```js
let data = await horse.dll.free(dllId);
console.log(data);
// {ok: true}
```
- 调用第三方库的方法
```js
let data = await horse.dll.invoke(dllId, "joinStr", "hello,");
console.log(data);
// {data: 'hello,world!', ok: true}
```

- 监听自定义事件
```js
let data = await horse.dll.on(dllId, "customEvent", (data) => {
  console.log("event", data);
});
```

- 取消监听自定义事件
```js
let data = await horse.dll.off(dllId, "customEvent");
console.log(data);
```