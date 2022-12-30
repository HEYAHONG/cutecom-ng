#include "qqmlloader.h"
#include "QQmlError"
#include "QList"
#include "QMessageBox"

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
}

QQuickWidget::Status QQmlLoader::GetLoadStatus()
{
    if(quick!=NULL)
    {
        return quick->status();
    }
    return QQuickWidget::Null;
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
    delete quick;
}
