## horse.dialog 的方法

- 打开路径选择对话框
```js
let data = await horse.dialog.openPath({
    title: "我的窗口标题",
    okBtnText: "选择文件",
    defaultPath: "C:\\Users",
    type: "file", //或"dir"，默认值为file
    filter: [
        ["All Files", "*.*"],
        ["Image Files", "*.jpg;*.png"],
    ], //只有type是file的时候才有效
    multiSelection: true, //默认值false
    showHiddenFile: true, //默认值false
});
console.log(JSON.stringify(data));
//输出：{"cancel":false,"data":["D:\\1.mp4","D:\\test.png"],"ok":true}
//或者：{"cancel":true,"ok":true}
```
- 打开路径保存对话框
```js
let data = await horse.dialog.savePath({
  title: "另存为...",
  okBtnText: "保存文件",
  defaultPath: "C:\\Users\\",
  filter: [["Image Files", "*.jpg;*.png"]], //只有type是file的时候才有效
  showHiddenFile: true, //默认值false
});
console.log(JSON.stringify(data));
//输出：{"cancel":false,"data":"C:\\Users\\111.png","ok":true}
```


- 显示一个MessageBox
```js

```