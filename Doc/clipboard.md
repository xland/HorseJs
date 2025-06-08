## horse.clipboard 的方法

- 获取剪切板内的数据的数据类型
> 已定义的数据类型：file、html、text、img、unknown
```js
let data = await horse.clipboard.getDataType();
console.log(data);
```

- 读取剪切板内的文本

```js
let data = await horse.clipboard.readText();
console.log(data);
// {ok: true, data: '剪切板内的文本'}
```

- 向剪切板内写入文本

```js
let data = await horse.clipboard.writeText("待写入的文本");
console.log(data);
// {ok: true}
```

- 读取剪切板内的Html字符串
```js
let data = await horse.clipboard.readHtml();
console.log(data);
```

- 向剪切板写入Html数据
```js
let data = await horse.clipboard.writeHtml(`<h1>Hello, World!</h1><p>中文</p>`);
console.log(data);
```

- 读取剪切板内的Rtf字符串
```js
let data = await horse.clipboard.readRtf();
console.log(data);
```