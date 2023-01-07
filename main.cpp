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
#include <QDir>

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
    {
        //切换工作目录（将当前工作目录切换到用户配置所在目录）
        QString configpath=QDir::homePath();
#ifdef WIN32
        //Windows下配置文件目录为 用户目录/AppData/Roaming/程序名
        configpath+="/AppData/Roaming/cutecom-ng";
#else
        //非Windows下配置文件目录为 用户目录/.config/程序名
        configpath+="/.config/cutecom-ng";
#endif
        QDir(QDir::homePath()).mkpath(configpath);
        QDir::setCurrent(configpath);

        qDebug()<<(QString("currentdir(configdir):")+QDir::currentPath());

    }
    MainWindow w;
    //设置翻译指针
    w.translator=&translator;
    w.show();

    return a.exec();
}
