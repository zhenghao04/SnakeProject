# snake1 - 基于 SDL2 的贪吃蛇游戏

**snake1** 是一款使用 _C++_ 编写、以 _SDL2_ 作为多媒体后端的经典贪吃蛇游戏。

<img src="https://habrastorage.org/webt/or/6-/_n/or6-_nko4hgztmhihnkxjw01e1e.gif" />

## 功能特点

* 支持窗口模式和全屏模式
* 可更改和自定义配色方案（也可以添加自己的方案）
* 可调节游戏设置（蛇的初始长度、游戏速度、游戏区域大小、无边界模式）
* 提供两种游戏模式：训练模式和生存模式

## 快捷启动

Windows 下可以直接双击仓库根目录中的 `start-game.cmd`。

脚本会自动执行 `Debug|x86` 增量构建，复制运行所需的 DLL 与资源文件，然后启动游戏。如果系统中没有安装 Visual Studio 或 MSBuild，但已经存在编译好的游戏文件，脚本也会直接启动现有版本。

## 构建与运行

1. 克隆本仓库；
2. 在 Visual Studio 中打开 `.sln` 文件（原项目使用 Visual Studio 2015）；
3. 将配置切换为 `x86`；
4. 构建项目；
5. 将 `.\external\lib\x86` 中的 DLL 文件复制到 `.\MSVS\Debug\Output`；
6. 将 `.\MSVS` 下的 `assets` 文件夹复制到 `.\MSVS\Debug\Output`；
7. 运行游戏。

## 待办事项

* 用户档案
* 最高分记录

## 状态

已冻结 :(
