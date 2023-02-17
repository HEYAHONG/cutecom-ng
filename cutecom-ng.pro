#-------------------------------------------------
#
# Project created by QtCreator 2015-06-10T16:52:08
#
#-------------------------------------------------

QT       += core gui serialport qml quick quickwidgets xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cutecom-ng
TEMPLATE = app
DESTDIR = bin
INCLUDEPATH += libs libs/qhexedit2/src/

OBJECTS_DIR = .generated/
MOC_DIR = .generated/
RCC_DIR = .generated/
UI_DIR = .generated/

SOURCES += main.cpp\
    aboutdialog.cpp \
    hexlistitem.cpp \
    mainwindow.cpp \
    connectdialog.cpp \
    nulltransfer.cpp \
    pluginmanager.cpp \
    qqmlloader.cpp \
    searchwidget.cpp \
    serialportstatus.cpp \
    sessionmanager.cpp \
    outputmanager.cpp \
    historycombobox.cpp \
    history.cpp \
    searchhighlighter.cpp \
    xmodemtransfer.cpp \
    filetransfer.cpp \
    libs/crc16.cpp \
    libs/xmodem.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    connectdialog.h \
    hexlistitem.h \
    nulltransfer.h \
    pluginmanager.h \
    qqmlloader.h \
    qscintillaedit.h \
    searchwidget.h \
    serialportstatus.h \
    sessionmanager.h \
    outputmanager.h \
    historycombobox.h \
    history.h \
    searchhighlighter.h \
    xmodemtransfer.h \
    filetransfer.h \
    libs/crc16.h \
    libs/xmodem.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    connectdialog.ui \
    hexlistitem.ui \
    pluginmanager.ui \
    searchwidget.ui \
    serialportstatus.ui

RESOURCES += \
    cutecom-ng.qrc

TRANSLATIONS+= \
                zh_CN.ts \
                en_US.ts

#解决MSVC UTF8编码问题
msvc:QMAKE_CXXFLAGS += -utf-8
msvc:QMAKE_CXXFLAGS_WARN_ON += -wd4819

CONFIG += debug_and_release
CONFIG(debug, debug|release){
    #处理debug
    win32{
        win32-msvc*{
            RC_FILE += win32/window_resources_msvc.rc
        }else{
            RC_FILE += win32/window_resources.rc
        }

    }
    unix{
    }
}else{
    #处理release
    win32{
        win32-msvc*{
            RC_FILE += win32/window_resources_msvc.rc
        }else{
            RC_FILE += win32/window_resources.rc
        }
    }
    unix{
    }
}



#包含components中的库
win32{
    COMPONENTS_PRI_DIRS = $$system(cmd.exe /c dir /A:D /B $$PWD/components/)
    for(component_dir,COMPONENTS_PRI_DIRS): {
        component_pri_file = $$files($$PWD/components/$${component_dir}/*.pri,false)
        include ($${component_pri_file})
    }

}

unix{
    COMPONENTS_PRI_DIRS = $$files($$PWD/components/*,false)
    for(component_dir,COMPONENTS_PRI_DIRS): {
        component_pri_file = $$files($${component_dir}/*.pri,false)
        include ($${component_pri_file})
    }
}

