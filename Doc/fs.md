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
> 从第10个字节开始，读取100个字节
```js
let data = await horse.fs.readFileChunk("D:\\index.html",10,100);
const uint8Array = new Uint8Array(data.buffer);
const decoder = new TextDecoder('utf-8');
const text = decoder.decode(uint8Array);
console.log('Decoded string:', text);
data.release();
```
- 写文件
> 文件存在，则覆盖；文件不存在，则创建
```js
await horse.fs.writeFile("D:\\abc.txt", "Hello World \n 你好，世界");
```

- 删除一个路径
> 路径可以是一个文件，也可以是一个目录
```js
//路径可以是一个文件，也可以是一个目录
await horse.fs.delPath("D:\\abc.txt");
```

- 把一个路径移入回收站
> 路径可以是一个文件，也可以是一个目录
```js
await horse.fs.delPath("D:\\abc.txt");
```

- 路径是否存在
```js
let data = await horse.fs.exists("D:\\index.html");
console.log(data);
// 输出：{isDir: false, isExists: true}
```

- 获取文件信息
> 返回结果里的时间数据，可以直接用`new Date(1748565302829)`格式化为 js 的时间格式
```js
let data = await horse.fs.getFileInfo("D:\\index.html");
console.log(data);
// 输出：{createTime:1747615428326,
//  fileSize:9190,
//  isHidden:false,
//  isReadOnly:false,
//  isSystem:false,
//  lastAccessTime:1748565302829,
//  lastWriteTime:1748565107113}
```
- 创建一个路径
```js
let data = await horse.fs.createDir("D:\\aaa");
console.log(data);
```

- 确保一个路径存在
> 如果路径中的子路径不存在，则创建
```js
let data = await horse.fs.ensurePath("D:\\aaa\\bbb\\ccc\\ddd.txt");
console.log(data);
```

- 列出一个路径下的所有子路径及文件（递归）
> 请尽量不要列出子项超多的路径，这可能会导致你的应用卡死
```js
let data = await horse.fs.listDir("D:\\project");
console.log(data);
```

- 移动一个路径
> 可以移动文件或路径
```js
let data = await horse.fs.movePath("D:\\doc\\allen.txt", "D:\\Doc2\\img\\");
console.log(data);
```

- 复制一个路径
```js
let data = await horse.fs.copyPath("D:\\doc\\allen\\", "D:\\project\\img\\");
console.log(data);
```

- 重命名一个路径
```js
let data = await horse.fs.renamePath("D:\\doc\\allen\\", "D:\\doc\\新目录\\");
console.log(data);
```

- 获取一个已知路径
>可选参数：<br />
> exeDir          <br />
> exePath	      <br />
> download	      <br />
> music		      <br />
> video		      <br />
> picture	      <br />
> document	      <br />
> startup	      <br />
> desktop	      <br />
> font		      <br />
> program	      <br />
> system	      <br />
> windows	      <br />
> profile	      <br />
> appdata	      <br />
> roaming	      <br />
> cache		      <br />
> userdata	      <br />
```js
let data = await horse.getPath("userdata");
console.log(JSON.stringify(data));
>> {"data":"C:\\Users\\liuxiaolun\\AppData\\Roaming\\","ok":true}
```

- 监视一个路径的变化
> 得到的data里有一个id属性，这个属性用于取消监视 <br />
> 当路径发生变化时，将输出：{file: '111.txt', ok: true, type: 'remove'} <br />
> type的值可能为：add、remove、modify、renaming、renamed、undefined <br />
```js
let data = await horse.fs.watch("D:\\doc\\周报\\allen\\", (data) => {
	console.log(data);
});
```
- 取消监听路径的变化
```js
let data = await horse.fs.stopWatch(watchId);
console.log(data);
```
- 创建一个快捷方式

```js
let data = await horse.fs.createShortcut("C:\\Windows\\System32\\cmd.exe", 
	"C:\\Users\\liuxiaolun\\Desktop\\cmd.lnk", 
	"命令行", 
	"C:\\Windows\\System32\\");
console.log(data);
```

- 使用默认应用打开一个文件路径。

```js
await horse.os.openFile("C:\\Recovery\\ReAgentOld.xml");
```