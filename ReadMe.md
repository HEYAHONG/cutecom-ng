# 说明

本工程为个人修改版。基于[cutecom-ng](doc/ReadMeOld.md),添加一些个人常用的功能

## 截图

![cutecom-ng-screenshot-1](doc/cutecom-ng-screenshot-1.png)

![cutecom-ng-screenshot-1](doc/cutecom-ng-screenshot-2.png)

## 插件

在使用串口工具时，经常需要对接收的串口数据进行处理，而这部分处理过程可能是多变的（如定时发送序列化的数据，使用串口数据进行绘图等）。

Qt的Qml技术即可满足此需求，数据处理过程可以以简单的方式修改且无需再次编译，也能提供丰富的GUI支持。

模板（插件加载时不关心后缀名）见：[script/qml/](script/qml/)

# 编译

安装好Qt5、SerialPort扩展及QtDeclarative。安装好qtcreator。

对于ubuntu可采用以下命令安装:

```bash
sudo apt-get install qtcreator libqt5serialport5-dev qtdeclarative5-dev
#如需使用Qt6,可使用以下命令替换上面的命令
sudo apt-get install qtcreator libqt6serialport6-dev qt6-declarative-dev libqt6core5compat6-dev
```

## 使用qtcreator编译

直接使用qtcreator打开cutecom-ng.pro。

## 使用make编译

使用以下命令生成makefile:

```bash
qmake -makefile cutecom-ng.pro
```

若命令成功，则可直接使用make编译：

```bash
make
```

编译完成后，可在bin目录中找到编译完成的二进制文件。

## 使用CMake编译

注意：本工程使用CMake编译时不推荐直接在源代码目录编译,建议另外创建目录用于构建。所有打包的安装包均使用CMake生成。

使用以下命令生成工程文件：

```bash
#Windows下使用以下命令
cmake -G "Ninja" [源代码目录]
#Linux下使用以下命令
cmake  [源代码目录]
```

使用以下命令开始编译：

```bash
#直接编译
ninja #Windows
make  #Linux
#打包安装包
ninja package #Windows
make package #ubuntu
```



# 运行

由于使用了qml模块，因此需要保证下列模块已安装：

- QtQuick
- QtQuick.Controls 
- QtQuick.Dialogs

对于ubuntu可采用以下命令安装Qml模块:

```bash
sudo apt-get install qml-module-qtquick-controls qml-module-qtquick-dialogs
#如需使用Qt6,可使用以下命令替换上面的命令
sudo apt-get install qml6-module-qtquick-controls qml6-module-qtquick-dialogs
```
