#include "qqmlloader.h"
#include "QQmlError"
#include "QList"
#include "QMessageBox"
#include "QQuickItem"

QQmlLoader::QQmlLoader(QWidget *parent):QDialog(parent)
{
    quick=new QQuickWidget(this);
    connect(quick,&QQuickWidget::statusChanged,this,&QQmlLoader::statusChanged);
    SetupQmlContext(quick->rootContext());
}


void QQmlLoader::LoadQmlSource(QUrl qml_path)
{
    quick->setSource(qml_path);
    resize(quick->geometry().size());
    QString name=GetPluginName();
    if(!name.isEmpty())
    {
        setWindowTitle(name);
        QQuickItem * root=quick->rootObject();
        //调用初始化函数
        QMetaObject::invokeMethod(root,"onplugininit");
    }
    else
    {
        qDebug()<< qml_path << " is not a plugin";
    }
}

QQuickWidget::Status QQmlLoader::GetLoadStatus()
{
    if(quick!=NULL)
    {
        return quick->status();
    }
    return QQuickWidget::Null;
}

QString QQmlLoader::GetPluginName()
{

    QVariant name=GetRootItemProperty("pluginname");
    if(!name.isNull())
    {
            return name.toString();
    }

    return QString();
}

QVariant QQmlLoader::GetRootItemProperty(QString name)
{
    if(quick->rootObject()!=NULL)
    {
        return quick->rootObject()->property(name.toStdString().c_str());
    }
    return QVariant();
}

void QQmlLoader::statusChanged(QQuickWidget::Status status)
{
    if(status==QQuickWidget::Error)
    {
        QString ErrorStr;
        QList<QQmlError>  errors=quick->errors();
        for(QList<QQmlError>::iterator it=errors.begin();it!=errors.end();it++)
        {
            QQmlError error=(*it);
            if(error.isValid())
            {
                ErrorStr+=QString(error.toString())+QString("\n");
            }
        }
        QMessageBox::critical(this,tr("Error"),ErrorStr);

        hide();
    }
}

void QQmlLoader::SetupQmlContext(QQmlContext *root)
{
    (void)root;
   /*
    *设置Qml环境。
    *主要设置一些qml脚本中需要用到的上下文环境
    *
    */

}

QQmlLoader::~QQmlLoader()
{
    QQuickItem * root=quick->rootObject();
    if(!GetPluginName().isEmpty())
    {
        //调用反初始化函数
        QMetaObject::invokeMethod(root,"onplugindeinit");
    }
    delete quick;
}
