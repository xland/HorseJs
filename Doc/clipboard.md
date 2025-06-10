## horse.clipboard 的方法

- 获取剪切板内的数据的数据类型
> 已定义的数据类型：file、html、text、img、rtf、unknown
```js
let data = await horse.clipboard.getDataType();
console.log(data);
//输出：{data: 'img', ok: true}
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
// {data: '<ul dir="auto">读取剪切板内的Html字符串</li></ul>', ok: true}
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
//输出：{data: '{\\rtf1\\adeflang1025\\ansi\\ansicpg936\\uc2\\adeff31507…000105000000000000}}', ok: true}
```

- 向剪切板写入Rtf字符串
```js
const rtfContent = "{\\rtf1\\ansi\\ansicpg65001\\deff0" +
  "{\\fonttbl{\\f0\\fnil\\fcharset134 SimSun;}}" +
  "{\\colortbl ;\\red255\\green0\\blue0;\\red0\\green0\\blue255;}" +
  "\\pard\\fs24" +
  "This is a \\b bold\\b0  and \\i italic\\i0  这是宋体中文文本。\\par" +
  "\\fs20 Normal text with default font.\\par" +
  "}";
let data = await horse.clipboard.writeRtf(rtfContent);
console.log(data);
```

- 读取剪切板内的文件路径
```js
let data = await horse.clipboard.getFile();
console.log(data);
//输出：{"data":["D:\\1.mp4","D:\\test.png"],"ok":true}
```

- 向剪切板写入文件
```js
let data = await horse.clipboard.addFile("D:\\window.html", 
"D:\\tray.html");
console.log(data);
```

- 读取剪切板内的图像
```js
let data = await horse.clipboard.readImg();
const uint8Array = new Uint8Array(data.buffer);
const binary = Array.from(uint8Array)
	.map((byte) => String.fromCharCode(byte))
	.join("");
const base64String = btoa(binary);
const base64Image = `data:image/png;base64,${base64String}`;
data.release();
console.log(base64Image);
```

- 向剪切板写入图像
```js
let data = await horse.clipboard.writeImg(`D:\\horse.png`);
console.log(data);
```

- 清空剪切板
```js
let data = await horse.clipboard.clear();
console.log(data);
```