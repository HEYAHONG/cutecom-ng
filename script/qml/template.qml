import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.1



Item {
    width: 320
    height: 240
    /*
     *属性名称：pluginname
     *属性说明: 插件名称。必须存在，否则将不判定为插件。
    */
    property string pluginname:"template"
    /*
     *属性名称：pluginversion
     *属性说明: 插件版本。可选。
    */
    property string pluginversion:"1.0.0"

    /*
     *说明:插件初始化。当插件被加载时由C++调用。必须存在。
     */
    function onplugininit()
    {
        //在此处初始化插件
        console.log("template plugin init");
    }
    /*
     *说明:插件反初始化。当插件被加载时由C++调用。必须存在。
     */
    function onplugindeinit()
    {
        //在此处反初始化插件
        console.log("template plugin deinit");
    }

}
