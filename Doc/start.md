## 快速开始

1. [下载 HorseJs](https://github.com/xland/HorseJs/releases)，并解压缩 Release.7z

1. 使用以下目录结构配置你的项目

```shell
├─ HorseJs.exe     # 你可以随意修改此文件的文件名，及其图标资源
├─ UI              # 此目录放置配置文件和HTML/CSS/JS文件，必须叫这个名字
   ├─ css/         # 网页样式目录，只要在UI目录下即可，命名无要求
   ├─ js/          # 网页脚本目录，只要在UI目录下即可，命名无要求
   ├─ index.html   # 网页入口文件，目前暂时只能叫这个名字
   ├─ config.json  # 应用程序配置文件，必须叫这个名字
```
3. 现在你可以启动HorseJs.exe查看你的应用了 
1. 你可以使用 [Resource Hacker](https://www.angusj.com/resourcehacker/) 修改 HorseJs.exe 的图标
1. 你可以使用 [Windows Installer](https://learn.microsoft.com/en-us/windows/win32/msi/windows-installer-portal), [NSIS](http://nsis.sourceforge.net/Main_Page) 或 [Inno Setup](http://www.jrsoftware.org/isinfo.php) 制作应用程序安装包


> 将来 HorseJs 将提供官方的打包工具（类似：electron-builder）以及资源困扎工具（用于生成单个可执行 exe 文件）