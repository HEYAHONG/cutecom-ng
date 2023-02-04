# 说明

编写QML插件与编写一般的QML程序一样。

注意事项如下:

- 由于是通过QQuickWidget加载的QML脚本，推荐使用Item作为根，不能使用ApplicationWindow作为根。
- 符合插件格式,即根对象需要定义相关属性。

## 示例

见本目录下的qmlscript文件。

|                      文件名                      |                  说明                   |
| :----------------------------------------------: | :-------------------------------------: |
|     [template.qmlscript](template.qmlscript)     | 模板插件。适用于Qt5。内置于cutecom-ng。 |
| [template-qt6.qmlscript](template-qt6.qmlscript) | 模板插件。适用于Q6。内置于cutecom-ng。  |



## 插件格式

QML脚本中根对象的要求如下:

| 类型 |       名称       |                             说明                             |          备注          |
| :--: | :--------------: | :----------------------------------------------------------: | :--------------------: |
| 属性 |    pluginname    |                  插件名称,字符串属性,必须。                  | 主窗口菜单中显示此名称 |
| 属性 |  pluginversion   |                  插件版本,字符串属性,可选。                  |                        |
| 函数 |  onplugininit()  |       插件初始化。当插件被加载时由C++调用。必须存在。        |                        |
| 函数 | onplugindeinit() | 插件反初始化。当插件被卸载时由C++调用(并非窗口被关闭)。必须存在。 |                        |



# QML插件中可使用的API

**注意:本节仅说明由cutecom-ng扩展的部分，QML本身的API均可正常使用**

QML中可使用JS可通过访问全局属性对象实现与cutecom-ng的交互。支持的全局属性对象如下:

- cutecomng:此对象用于访问串口及定时器相关内容。

## 全局属性对象

### cutecomng

cutecomng对象成员如下：

|       成员名称        | 类型 |       说明       |                 备注                 |
| :-------------------: | :--: | :--------------: | :----------------------------------: |
|       QtVersion       | 属性 | Qt版本,字符串。  |                                      |
|   SetTimerCallback    | 函数 |  设置定时器回调  |                                      |
|      StartTimer       | 函数 |    启动定时器    |                                      |
|       StopTimer       | 函数 |    停止定时器    |                                      |
|     IsSerialOpen      | 函数 |   串口是否打开   |                                      |
| SetSerialDataCallback | 函数 | 设置串口数据回调 |             用于接收数据             |
|    SendSerialData     | 函数 |   发送串口数据   | 必须在串口处于打开状态时才能成功发送 |

#### SetTimerCallback

返回值无,参数一为回调函数。

回调函数的返回值与参数均无。

#### StartTimer

返回值无,参数一为定时器超时间隔,单位毫秒。

#### StopTimer

返回值与参数均无。

#### IsSerialOpen

返回值为是否串口被打开,参数无。

#### SetSerialDataCallback

返回值无,参数一为回调函数。

回调函数的返回值无，参数一为接收到的数据（字符串）。

#### SendSerialData

返回值为是否发送成功,参数一待发送的数据。