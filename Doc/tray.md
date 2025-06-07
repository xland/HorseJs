- 创建托盘图标（及其关联的菜单）
```js
await horse.tray.create({
    rightBtnDown: () => {
        console.log("rightBtnDown");
    },
    leftBtnDown: () => {
        console.log("leftBtnDown");
    },
    menu: [
        {
        text: "退出",
        click: () => {
            console.log("tray menu item click");
        },
        },
    ],
});
```