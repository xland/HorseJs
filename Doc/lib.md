用于加载第三方库

- 加载第三方库
```js
let data = await horse.lib.load("./add.dll");
console.log(data)
// {ok: true,id:1}
```
- 释放第三方库
```js
let data = await horse.lib.free(1);
console.log(data)
// {ok: true}
```
- 调用第三方库的方法
```js
let data = await horse.lib.call(1,"add","[1,2]");
console.log(data)
// {ok: true,data:"3"}
```