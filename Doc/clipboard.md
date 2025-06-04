## horse.clipboard 的方法

- 读取剪切板内的文本

```js
let data = await horse.clipboard.readText();
console.log(data);
// {ok: true, result: '剪切板内的文本'}
```

- 向剪切板内写入文本

```js
let data = await horse.clipboard.writeText("待写入的文本");
console.log(data);
// {ok: true}
```