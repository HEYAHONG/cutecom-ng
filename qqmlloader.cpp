#include "qqmlloader.h"
#include "QQmlError"
#include "QList"
#include "QMessageBox"
#include "QQuickItem"
#include "QQmlContext"

QQmlLoader::QQmlLoader(QWidget *parent):QDialog(parent),QmlTimer(this)
{
    quick=new QQuickWidget(this);
    connect(quick,&QQuickWidget::statusChanged,this,&QQmlLoader::statusChanged);
    connect(&QmlTimer,&QTimer::timeout,this,&QQmlLoader::TimerTimeout);
    SetupQmlContext(quick->rootContext());
}


void QQmlLoader::LoadQmlSource(QUrl qml_path)
{
    quick->setSource(qml_path);
    resize(quick->geometry().size());
    setMaximumSize(size());
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

QString QQmlLoader::getQtVersion()
{
    return QString(qVersion());
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

    /*
     *将本类对象传入qml上下文，名称为cutecomng
    */
    root->setContextProperty("cutecomng",this);

}

void QQmlLoader::StartTimer(int interval)
{
    QmlTimer.setSingleShot(false);
    QmlTimer.start(interval);
}

void QQmlLoader::StopTimer()
{
    QmlTimer.stop();
}

void QQmlLoader::SetTimerCallback(QJSValue callback)
{
    QmlTimerCallback=callback;
}

void QQmlLoader::TimerTimeout()
{
    QMutexLocker lock(&QmlCallLock);

    //调用回调函数
    QJSValue callback=QmlTimerCallback;
    if(callback.isCallable())
    {
        callback.call();
    }
}

QQmlLoader::~QQmlLoader()
{
    QmlTimer.stop();
    QQuickItem * root=quick->rootObject();
    if(!GetPluginName().isEmpty())
    {
        //调用反初始化函数
        QMetaObject::invokeMethod(root,"onplugindeinit");
    }
    delete quick;
}
