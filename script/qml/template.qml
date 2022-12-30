import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.1



Item {
    width: 320
    height: 240
    /*!
     *属性名称：pluginname
     *属性说明: 插件名称。必须存在，否则将不判定为插件。
    */
    property string pluginname:"template"
    /*!
     *属性名称：pluginversion
     *属性说明: 插件版本。可选。
    */
    property string pluginversion:"1.0.0"

}
