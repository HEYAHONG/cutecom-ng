/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief cutecom-ng entry point
 *
 * \author Aurelien Rainone <aurelien@develer.com>
 */

#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QTranslator>
#include <QDebug>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));
    QTranslator translator;
    {
        //加载翻译
        QLocale loc;
        qDebug()<<QString("locale:")<<loc;
        switch(loc.country())
        {
        case loc.China:
            {
                translator.load(":/zh_CN.qm");
            }
            break;
        default:
            {
                translator.load(":/en_US.qm");
            }
            break;
        }
        a.installTranslator(&translator);
    }
    MainWindow w;
    //设置翻译指针
    w.translator=&translator;
    w.show();

    return a.exec();
}
