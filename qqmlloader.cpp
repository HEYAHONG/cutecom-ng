#include "qqmlloader.h"


QQmlLoader::QQmlLoader(QWidget *parent):QDialog(parent)
{
    quick=new QQuickWidget(this);
    SetupQmlContext(quick->rootContext());
}


void QQmlLoader::LoadQmlSource(QUrl qml_path)
{
    quick->setSource(qml_path);
    resize(quick->geometry().size());
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
