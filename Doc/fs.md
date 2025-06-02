## horse.fs 的方法

- 读取文件全部内容
```js
let data = await horse.fs.readFile("D:\\index.html");
const uint8Array = new Uint8Array(data.buffer); // 字节视图
const decoder = new TextDecoder('utf-8'); // UTF-8 解码器
const text = decoder.decode(uint8Array); // 转换为字符串
console.log('Decoded string:', text);
data.release(); //释放资源
```
- 读取文件的一部分
```js
//从第10个字节开始，读取100个字节
let data = await horse.fs.readFileChunk("D:\\index.html",10,100);
const uint8Array = new Uint8Array(data.buffer);
const decoder = new TextDecoder('utf-8');
const text = decoder.decode(uint8Array);
console.log('Decoded string:', text);
data.release();
```
- 写文件
```js
//文件存在，则覆盖；文件不存在，则创建
await horse.fs.writeFile("D:\\abc.txt", "Hello World \n 你好，世界");
```

- 删除一个路径
```js
//路径可以是一个文件，也可以是一个目录
await horse.fs.delPath("D:\\abc.txt");
```

- 把一个路径移入回收站
```js
//路径可以是一个文件，也可以是一个目录
await horse.fs.delPath("D:\\abc.txt");
```

- 路径是否存在
```js
let data = await horse.fs.exists("D:\\index.html");
console.log(data);
// 输出：{isDir: false, isExists: true}
```
- 获取文件信息
```js
let data = await horse.fs.getFileInfo("D:\\index.html");
console.log(data);
// 输出：{createTime:1747615428326,fileSize:9190,isHidden:false,isReadOnly:false,isSystem:false,lastAccessTime:1748565302829,lastWriteTime:1748565107113}
```
## horse.fs 的事件
