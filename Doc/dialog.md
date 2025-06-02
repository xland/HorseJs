## horse.dialog 的方法

- 打开路径对话框
```js
let data = await horse.dialog.openPathDialog({
  title: "我的窗口标题",
  okBtnText: "选择文件",
  defaultDir: "C:\\Users\\liulun\\AppData\\Roaming\\HorseJs",
  type: "file", //或"dir"，默认值为file
  filter: [
    ["All Files", "*.*"],
    ["Image Files", "*.jpg;*.png"],
  ], //只有type是file的时候才有效
  multiSelection: true, //默认值false
  showHiddenFile: true, //默认值false
});
console.log(data);
```
