# XLEX黑窗代码

- 正则表达式转NFA再转DFA再转MDFA的核心代码，可以基于这里的核心代码在QT开发GUI。

## 一、编译方法

### 1.1 手动编译

下载Mingw64/Clang++等编译器，在vscode中配置生成任务或直接手动shell运行：

```shell
g++ -g -std=c++11 *.cpp -o main
```

即可输出二进制文件。具体的Debug方法可以看Google，不推荐手动shell，可以在vscode配置生成任务或使用下面1.4的Debug方案。

### 1.2 CMake

- 需要CMake，macOS可通过Homebrew下载，Windows可通过官网下载。

1. 在项目目录下执行：

    ```shell
    cmake -B build .
    ```

    将会在`build`文件夹中产生输出

2. 进入`build`文件夹，macOS下执行`make`，Windows下执行`msbuild`（Windows需要已安装Visual Studio Tools）


### 1.3 CMake + Ninja

编译方法1.2并不是一个优雅的方式，因为其在macOS和Windows下的CMake构建产物有所差别，消除这种差别的终极方案是使用Ninja，这需要先安装好Ninja。同样的，macOS推荐使用Homebrew安装Ninja，Windows自行在Google搜索Ninja官网安装。

1. 在项目目录下执行：

    ```shell
    cmake -B build -G Ninja . 
    ```
2. 在`build`目录下执行

    ```shell
    ninja
    ```

即可产生可执行文件。

### 1.4 CMake + vscode

首先需要安装好CMake，vscode安装CMake Tools插件。

然后直接根据CMake Tools插件的配置指引一路操作，可以在面板**状态栏**左下角看到`生成`、`Debug`等操作按钮，点击即可生成或Debug。

## 二、运行指南

因为这里只是核心功能代码，不包括任何输入输出、测试方法、GUI逻辑，所以你可以在`main.cpp`中的`return`语句前打上断点，通过Debug查看NFA、DFA、MDFA的生成结果。