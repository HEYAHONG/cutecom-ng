#-------------------------------------------------
#
# Project created by QtCreator 2015-06-10T16:52:08
#
#-------------------------------------------------

QT       += core gui serialport qml quick quickwidgets

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
    libs/qhexedit2/src/chunks.cpp \
    libs/qhexedit2/src/commands.cpp \
    libs/qhexedit2/src/qhexedit.cpp \
        mainwindow.cpp \
    connectdialog.cpp \
    pluginmanager.cpp \
    qqmlloader.cpp \
    searchwidget.cpp \
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
    connectdialog.h \
    libs/qhexedit2/src/chunks.h \
    libs/qhexedit2/src/commands.h \
    libs/qhexedit2/src/qhexedit.h \
    pluginmanager.h \
    qqmlloader.h \
    searchwidget.h \
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
    connectdialog.ui \
    pluginmanager.ui \
    searchwidget.ui

RESOURCES += \
    cutecom-ng.qrc

TRANSLATIONS+= \
                zh_CN.ts \
                en_US.ts

CONFIG += debug_and_release
CONFIG(debug, debug|release){
    //处理debug
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
    //处理release
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

