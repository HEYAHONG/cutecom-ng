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
#ifdef WIN32
    //Qt自身的翻译
    QTranslator qt_translator;
    QString     qt_translator_base_dir;
    {
        QDir translator_dir(a.applicationDirPath()+"/../translations/");
        if(translator_dir.exists())
        {
            qt_translator_base_dir=translator_dir.path();
        }
    }
    {
        QDir translator_dir(a.applicationDirPath()+"/translations/");
        if(translator_dir.exists())
        {
            qt_translator_base_dir=translator_dir.path();
        }
    }
#endif
    {
        //加载翻译
        QLocale loc;
        qDebug()<<QString("locale:")<<loc;
        switch(loc.country())
        {
        case loc.China:
            {
                translator.load(":/zh_CN.qm");
#ifdef WIN32
                if(!qt_translator_base_dir.isEmpty())
                {
                    qt_translator.load(qt_translator_base_dir+"/qt_zh_CN.qm");
                }
#endif
            }
            break;
        default:
            {
                translator.load(":/en_US.qm");
#ifdef WIN32
                if(!qt_translator_base_dir.isEmpty())
                {
                    qt_translator.load(qt_translator_base_dir+"/qt_en.qm");
                }
#endif
            }
            break;
        }
        a.installTranslator(&translator);
#ifdef WIN32
        a.installTranslator(&qt_translator);
#endif
    }    
    MainWindow w;
    //设置翻译指针
    w.translator=&translator;
    //设置配置目录
    {
        //切换工作目录（将当前工作目录切换到用户配置所在目录）
        QString configpath=QDir::homePath();
#ifdef WIN32
        //Windows下配置文件目录为 用户目录/AppData/Roaming/程序名
        configpath+="/AppData/Roaming/cutecom-ng/";
#else
        //非Windows下配置文件目录为 用户目录/.config/程序名
        configpath+="/.config/cutecom-ng/";
#endif
        QDir(QDir::homePath()).mkpath(configpath);
        QDir::setCurrent(configpath);

        qDebug()<<(QString("currentdir(configdir):")+QDir::currentPath());
        qDebug()<<(QString("tmpdir:")+QDir::tempPath());

        w.SetConfigPath(configpath);
    }

    w.show();

    return a.exec();
}
