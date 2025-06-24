## 剪切板

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
> 如下示例提供了两种使用image buffer数据的方式：<br />
> 使用objectURL读取图像数据、把图像数据格式化为 base64 字符串再使用 <br />
> 推荐第一种，因为占用内存更小，处理和渲染速度都更快。<br />
> 无论使用哪一种方案，都不要忘记 data.release(); 释放数据。
```js
let data = await horse.clipboard.readImg();
if (!data.ok) {
	console.log(data.err);
	return;
}
const uint8Array = new Uint8Array(data.buffer); // 字节视图
// objectURL读取图像（推荐）
const blob = new Blob([uint8Array], { type: "image/png" });
const url = URL.createObjectURL(blob);
// base64 字符串设置图像
// const binary = Array.from(uint8Array)
//   .map((byte) => String.fromCharCode(byte))
//   .join("");
// const base64String = btoa(binary);
// const url = `data:image/png;base64,${base64String}`;
data.release();
$("#imgIns").setAttribute("src", url);
$("#imgIns").setAttribute("style", `width: ${data.w}px; height: ${data.h}px;`);
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