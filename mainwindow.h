/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief main cutecom-ng window
 *
 * \author Aurelien Rainone <aurelien@develer.com>
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "filetransfer.h"
#include "qhexedit.h"
#include "Version.h"
#include "qqmlloader.h"
#include <QMainWindow>
#include <QTranslator>
#include <QDebug>
#include <QSerialPortInfo>
#include <QLabel>
#include <QMap>
#include <QSharedPointer>
#include <QVBoxLayout>
#include <QList>
#include <QDomDocument>

namespace Ui {
class MainWindow;
}

class SessionManager;
class OutputManager;
class ConnectDialog;
class QLineEdit;
class QToolButton;
class QProgressDialog;
class PluginManager;

/**
 * \brief main cutecom-ng window
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    /**
     * \brief different end of line char(s) that can be appended
     * to every line sent to the serial port
     */
    enum EndOfLineChar
    {
        None    = 0,    /// none, line is sent as-is
        LF      = 1,    /// LineFeed:       [0x0a]
        CR      = 2,    /// CarriageReturn: [0x0d]
        CRLF    = 3     /// CR + LF:        [0x0d, 0x0a]
    };

private:
    Ui::MainWindow      *ui;
    SessionManager      *session_mgr;
    OutputManager       *output_mgr;
    ConnectDialog       *connect_dlg;
    QWidget             *search_widget;
    QLineEdit           *search_input;
    QToolButton         *search_prev_button;
    QToolButton         *search_next_button;
    QProgressDialog     *progress_dialog;
    QByteArray          _end_of_line;

    //翻译
    QTranslator *translator;
    friend int main(int argc, char *argv[]);

    //状态栏
    QLabel *rightstatus;
    QLabel *rightstatus_2;

    //HEX输出列表
    QVBoxLayout *HexList;
    QList<QWidget *> HexWidgetList;

    //qml脚本
    QMap<QUrl,QSharedPointer<QQmlLoader>> qml_list;
    QMenu *qml_plugin_menu;

    //配置路径
    QString configpath;

    //配置文件（使用Dom管理配置）
    QDomDocument configdoc;

    //加载配置文件与保存配置文件
    void load_configdoc();
    void save_configdoc();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    friend class PluginManager;

    /*
     * 加载QML插件
     *
    */
    bool LoadQmlPlugin(QUrl qml_path,bool Show=true);

    /*
     * 卸载QML插件
     *
    */
    bool UnloadQmlPlugin(QUrl qml_path);

    /*
     *获取SessionManager指针
    */
    SessionManager * GetSessionManager();

    /*
     *设置配置路径目录（通常在main中调用）
    */
    void SetConfigPath(QString _path);

    /*
     *获取配置路径目录
    */
    QString GetConfigPath();

    /*
     *获取配置
    */
    QDomDocument& GetConfigDoc();

    /*
     *获取配置根节点
    */
    QDomElement GetConfigRootNode();

private slots:

    void sessionManager_statisticChanged(uint64_t bytesRead,uint64_t bytesWrite);

    void on_actionAbout_Qt_triggered();

    void on_action_Exit_triggered();

    void on_actionEnglish_triggered();

    void on_actionChinese_triggered();

    void on_RTScheckBox_stateChanged(int arg1);

    void on_DTRcheckBox_stateChanged(int arg1);

    void on_actionhexoutput_triggered(bool checked);

    void on_clearButton_clicked();

    void on_actionLoadQml_triggered();

    void on_actionpluginmanager_triggered();

    void on_actionmain_output_triggered();

    void on_actionvt100_output_triggered(bool checked);

    void on_actionversion_triggered();

private:

    /**
     * \brief handle sessionOpened signal
     */
    void handleSessionOpened();

    /**
     * \brief handle sessionClosed signal
     */
    void handleSessionClosed();

    /**
     * \brief handle buttonClicked on the x/y/zmodem buttons
     * \param type
     */
    void handleFileTransfer();

    /**
     * \brief handle new input
     */
    void handleNewInput(QString entry);

    /**
     * \brief add data to the output view
     */
    void addDataToView(const QString & textdata);

    /**
     * \brief handle arrival of new data
     */
    void handleDataReceived(const QByteArray &data);

    /**
     * \brief toggle bottom output text window and splitter
     */
    void toggleOutputSplitter();

    /**
     * \brief event filter
     */
    bool eventFilter(QObject *target, QEvent *event);

    /**
     * \brief show search widget
     * \param show show or hide?
     */
    void showSearchWidget(bool show);

    /**
     * \brief handle search result cursor position changes
     * \param pos pos of search result cursor
     */
    void handleCursosPosChanged(int pos);

    /**
     * \brief handle changes of number of search string occurences
     * \param total_occurences
     */
    void handleTotalOccurencesChanged(int total_occurences);

    /**
     * \brief handle end of file transfer
     * \param error transfer error code
     */
    void handleFileTransferEnded(FileTransfer::TransferError error);

    /**
     * \brief handle fileTransferProgressed signal
     * \param percent transfer current progression
     */
    void handleFileTransferProgressed(int percent);

    /**
     * \brief handle currentIndexChanged for end of line char combobox
     * \param index index of selected item
     */
    void handleEOLCharChanged(int index);
protected:
     void paintEvent(QPaintEvent *event);
     void keyPressEvent(QKeyEvent *event);
};

#endif // MAINWINDOW_H
