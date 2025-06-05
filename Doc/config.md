应用程序的配置文件 `config.json` 应保存在 UI 目录下。

- appId
> 类型：字符串，必须是英文，不能包含特殊符号 <br />
> 应用程序的Id。
- quitWhenNoWindow
> 类型：布尔，默认值：`true` <br />
> 当所有窗口关闭时，退出应用程序。
- window.resizable
> 类型：布尔，默认值：`true` <br />
> 是否允许调整窗口的大小。
 - window.size
> 默认值：`{w:1000,h:800}` <br />
> 值为：`"maximize"` 时，窗口将在当前屏幕全屏显示，但不覆盖任务栏 <br />
> 值为：`"fullScreen"` 时，窗口将在当前屏幕全屏显示，覆盖任务栏 <br />
> 值为：`"fullDesktop"` 时，窗口将覆盖所有屏幕并全屏显示，覆盖任务栏 <br />
> 值为：`{w:1000,h:800}`时，窗口将按指定大小显示。
 - window.miniSize
> 默认值：`{w:250,h:200}`  <br />
> 设置窗口的最小大小。
 - window.maxSize
> 示例值：`{w:2000,h:1600}`  <br />
> 设置窗口的最大大小。
 - window.position
> 默认值：`"centerScreen"`  <br />
> 值为：`"centerScreen"` 时,窗口将出现在当前屏幕正中央  <br />
> 值为：`{x:100,y:100}` 时，窗口将在指定位置显示 。
 - window.visible
> 类型：布尔，默认值：`true`。  <br />
> 值为：`true` 时，创建窗口即显示窗口，无论窗口是否加载完HTML。  <br />
> 值为：`false` 时，窗口创建时不显示，你可以在加载完HTML之后，再手动控制显示窗口。
 - window.frame
> 类型：布尔，默认值：`true`  <br />
> 窗口是否拥有边框，如果为true，则窗口具备标题栏和边框，是一个普通窗口。
 - window.shadow
> 类型：布尔，默认值：`true`。  <br />
> 窗口是否拥有有阴影，window.frame 为 true 时，此属性无效。
 - window.minimizable
> 类型：布尔，默认值：`true`。  <br />
> 是否允许窗口最小化
 - window.maximizable 
> 类型：布尔，默认值：`true`。  <br />
> 是否允许窗口最大化
 - window.alwaysOnTop
> 类型：布尔，默认值：`false`。  <br />
> 是否置顶窗口
 - window.skipTaskbar
> 类型：布尔，默认值：`true`。  <br />
> 是否置在任务栏中显示
 - window.transparent 
> 类型：布尔，默认值：`false`。  <br />
> 窗口是否背景透明，window.frame 为 true 时，此属性无效。
- window.webview.url
> 类型：字符串，默认值：`index.html`  <br />
> 实际会被格式化为：`"https://{appId}/index.html"`
 - window.webview.allowWindowOpen
> 类型：布尔，默认值：`true`。  <br />
> 是否允许在页面中打开新窗口。
 - window.title
> 类型：字符串，默认值：`Window - HorseJs`。  <br />
> 窗口标题，HTML的title将覆盖此设置。
- window.scriptDialogEnable
> 是否允许脚本打开alert、confirm等对话框
- window.scriptEnable
> 是否允许窗口中的页面执行脚本
- window.webMessageEnable
> 是否允许窗口中的页面与C++后端通信
- window.contextMenuEnable
> 是否启用窗口上的右键菜单