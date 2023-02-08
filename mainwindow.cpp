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

#include <algorithm>
#include <iterator>

//#include <QUiLoader>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QShortcut>
#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include "sessionmanager.h"
#include "outputmanager.h"
#include "searchhighlighter.h"
#include "searchwidget.h"
#include "pluginmanager.h"
#include "hexlistitem.h"

/// maximum count of document blocks for the bootom output
const int MAX_OUTPUT_LINES = 100;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    search_widget(0),
    search_input(0),
    progress_dialog(0),
    translator(NULL),
    rightstatus(NULL),
    rightstatus_2(NULL),
    qml_plugin_menu(NULL)
{
    ui->setupUi(this);

    //HexList的layout
    HexList=new QVBoxLayout(ui->hexOutputList);
    //隐藏hex输出
    ui->hexOutput->setVisible(false);

    //设置VT100输出
    ui->VT100Output->setVisible(false);
    ui->VT100Output->setEcho(false);

    // create session and output managers
    output_mgr = new OutputManager(this);
    session_mgr = new SessionManager(this);
    connect_dlg = new ConnectDialog(this);

    {
        //设置图标
        QIcon icon;
        icon.addFile(":/serial-port.png");
        setWindowIcon(icon);
    }

    {
        //初始化状态栏
        QStatusBar *qstBar = this->statusBar();
        if(qstBar!=NULL)
        {
            rightstatus=new QLabel(this);
            qstBar->addPermanentWidget(rightstatus);
            rightstatus_2=new QLabel(this);
            qstBar->addPermanentWidget(rightstatus_2);
        }
    }

    {
        qml_plugin_menu=new QMenu(tr("qml plugin"));
        ui->menuplugin->addMenu(qml_plugin_menu);
        //加载内置插件
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
        LoadQmlPlugin(QUrl("qrc:/script/qml/template-qt6.qmlscript"),false);
#else
        LoadQmlPlugin(QUrl("qrc:/script/qml/template.qmlscript"),false);
#endif

    }

    setFocusPolicy(Qt::StrongFocus);


    // show connection dialog
    connect(ui->connectButton, &QAbstractButton::clicked, connect_dlg, &ConnectDialog::show);

    // handle reception of new data from serial port
    connect(session_mgr, &SessionManager::dataReceived, this, &MainWindow::handleDataReceived);
    // handle  serial port statistic changed
    connect(session_mgr,&SessionManager::statisticChanged,this,&MainWindow::sessionManager_statisticChanged);

    // get data formatted for display and show it in output view
    connect(output_mgr, &OutputManager::dataConverted, this, &MainWindow::addDataToView);

    // get data formatted for display and show it in output view
    connect(ui->inputBox, &HistoryComboBox::lineEntered, this, &MainWindow::handleNewInput);

    // handle start/stop session
    connect(session_mgr, &SessionManager::sessionOpened, this, &MainWindow::handleSessionOpened);
    connect(session_mgr, &SessionManager::sessionClosed, this, &MainWindow::handleSessionClosed);

    // clear both output text when 'clear' is clicked
    connect(ui->clearButton, &QPushButton::clicked, ui->mainOutput, &QPlainTextEdit::clear);
    connect(ui->clearButton, &QPushButton::clicked, ui->bottomOutput, &QPlainTextEdit::clear);

    // connect open/close session slots
    connect(connect_dlg, &ConnectDialog::openDeviceClicked, session_mgr, &SessionManager::openSession);
    connect(ui->disconnectButton, &QPushButton::clicked, session_mgr, &SessionManager::closeSession);

    connect(ui->splitOutputBtn, &QPushButton::clicked, this, &MainWindow::toggleOutputSplitter);

    // load search widget and hide it
    {
        search_widget = new SearchWidget(ui->mainOutput);
        search_input = search_widget->findChild<QLineEdit*>("searchInput");
        search_prev_button = search_widget->findChild<QToolButton*>("previousButton");
        search_next_button = search_widget->findChild<QToolButton*>("nextButton");
    }
    search_widget->hide();

    // create the search results highlighter for main output
    SearchHighlighter *search_highlighter_main = new SearchHighlighter(ui->mainOutput->document());
    connect(search_input, &QLineEdit::textChanged, search_highlighter_main, &SearchHighlighter::setSearchString);

    // search result highlighter (without search cursor) for bottom output
    SearchHighlighter *search_highlighter_bottom = new SearchHighlighter(ui->bottomOutput->document(), false);
    connect(search_input, &QLineEdit::textChanged, search_highlighter_bottom, &SearchHighlighter::setSearchString);

    // connect search-related signals/slots
    connect(search_prev_button, &QPushButton::clicked,
	search_highlighter_main, &SearchHighlighter::previousOccurence);
    connect(search_next_button, &QPushButton::clicked,
	search_highlighter_main, &SearchHighlighter::nextOccurence);
    connect(search_highlighter_main, &SearchHighlighter::cursorPosChanged,
	this, &MainWindow::handleCursosPosChanged);
    connect(search_highlighter_main, &SearchHighlighter::totalOccurencesChanged,
	this, &MainWindow::handleTotalOccurencesChanged);
    connect(ui->searchButton, &QPushButton::toggled, this, &MainWindow::showSearchWidget);

    // additional configuration for bottom output
    ui->bottomOutput->hide();
    ui->bottomOutput->document()->setMaximumBlockCount(MAX_OUTPUT_LINES);

    //主输出设置最大block数量（设置一个较大的值防止内存不足）
    ui->mainOutput->setMaximumBlockCount(1000000);

    // populate file transfer protocol combobox
    ui->protocolCombo->addItem("XModem", SessionManager::XMODEM);
    ui->protocolCombo->addItem("YModem", SessionManager::YMODEM);
    ui->protocolCombo->addItem("ZModem", SessionManager::ZMODEM);

    // transfer file over XModem protocol
    connect(ui->fileTransferButton, &QPushButton::clicked, this, &MainWindow::handleFileTransfer);
    connect(session_mgr, &SessionManager::fileTransferEnded, this, &MainWindow::handleFileTransferEnded);

    // fill end of line chars combobox
    ui->eolCombo->addItem(QStringLiteral("CR"), CR);
    ui->eolCombo->addItem(QStringLiteral("LF"), LF);
    ui->eolCombo->addItem(QStringLiteral("CR+LF"), CRLF);
    ui->eolCombo->addItem(QStringLiteral("None"), None);
    ui->eolCombo->currentData(LF);
    _end_of_line = QByteArray("\n", 1);

    connect(ui->eolCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::handleEOLCharChanged);

    // install event filters
    ui->mainOutput->viewport()->installEventFilter(this);
    ui->bottomOutput->viewport()->installEventFilter(this);
    search_input->installEventFilter(this);
    installEventFilter(this);
}

void MainWindow::load_configdoc()
{
    {
        QFile file(GetConfigPath()+"config.xml");
        file.open(QFile::ReadOnly);
        if(file.isOpen())
        {
            if(!configdoc.setContent(file.readAll()))
            {
                qDebug()<<QString("load config.xml failed!");
            }
            file.close();
        }
    }

    //创建根节点,标签名为cutecom-ng
    QDomElement root=GetConfigRootNode();
    if(root.isNull())
    {
        QDomProcessingInstruction xmlInstruction  = configdoc.createProcessingInstruction("xml"," version = \"1.0\" encoding =\"UTF-8\" standalone=\"yes\" ");
        configdoc.appendChild(xmlInstruction);
        QDomElement root=configdoc.createElement("cutecom-ng");
        configdoc.appendChild(root);
    }

    {
        //加载mainwindow参数
        QDomDocument &doc=GetConfigDoc();
        QDomElement docroot=GetConfigRootNode();
        if(docroot.firstChildElement("mainwindow").isNull())
        {
            docroot.appendChild(doc.createElement("mainwindow"));
        }
        QDomElement root=docroot.firstChildElement("mainwindow");
        if(!root.isNull())
        {
            {
                //设置菜单
                if(root.firstChildElement("menusettings").isNull())
                {
                    root.appendChild(doc.createElement("menusettings"));
                }
                QDomElement menusettings=root.firstChildElement("menusettings");
                if(menusettings.hasAttribute("direntinput"))
                {
                    ui->actiondirentinput->setChecked(menusettings.attribute("direntinput").toInt()!=0);
                }
                if(menusettings.hasAttribute("hexoutput"))
                {
                    ui->actionhexoutput->setChecked(menusettings.attribute("hexoutput").toInt()!=0);
                    on_actionhexoutput_triggered(ui->actionhexoutput->isChecked());
                }
                if(menusettings.hasAttribute("mainoutput"))
                {
                    ui->actionmain_output->setChecked(menusettings.attribute("mainoutput").toInt()!=0);
                    on_actionmain_output_triggered();
                }
                if(menusettings.hasAttribute("vt100output"))
                {
                    ui->actionvt100_output->setChecked(menusettings.attribute("vt100output").toInt()!=0);
                    on_actionvt100_output_triggered(ui->actionvt100_output->isChecked());
                }

            }
        }

    }


    //加载connectdialog的设置
    if(connect_dlg!=NULL)
    {
        connect_dlg->loadconfig();
    }
}
void MainWindow::save_configdoc()
{
    {
        //保存mainwindow参数
        QDomDocument &doc=GetConfigDoc();
        QDomElement docroot=GetConfigRootNode();
        if(docroot.firstChildElement("mainwindow").isNull())
        {
            docroot.appendChild(doc.createElement("mainwindow"));
        }
        QDomElement root=docroot.firstChildElement("mainwindow");
        if(!root.isNull())
        {
            {
                //设置菜单
                if(root.firstChildElement("menusettings").isNull())
                {
                    root.appendChild(doc.createElement("menusettings"));
                }
                QDomElement menusettings=root.firstChildElement("menusettings");
                menusettings.setAttribute("direntinput",ui->actiondirentinput->isChecked());
                menusettings.setAttribute("hexoutput",ui->actionhexoutput->isChecked());
                menusettings.setAttribute("mainoutput",ui->actionmain_output->isChecked());
                menusettings.setAttribute("vt100output",ui->actionvt100_output->isChecked());

            }
        }

    }

    QFile file(GetConfigPath()+"config.xml");
    file.open(QFile::WriteOnly);
    if(file.isOpen())
    {
        file.write(configdoc.toByteArray());
        file.close();
    }
}

MainWindow::~MainWindow()
{
    save_configdoc();
    if(session_mgr->isSessionOpen())
        session_mgr->closeSession();
    delete ui;
}

void MainWindow::handleSessionOpened()
{
    // clear output buffer
    output_mgr->clear();

    // clear both output windows
    ui->mainOutput->clear();
    ui->bottomOutput->clear();

    ui->connectButton->setDisabled(true);
    ui->disconnectButton->setEnabled(true);

    // enable file transfer and input line
    ui->fileTransferButton->setEnabled(true);
    ui->inputBox->setEnabled(true);

    //启用RTS与DTR勾选框
    ui->RTScheckBox->setEnabled(true);
    ui->DTRcheckBox->setEnabled(true);

    {
        //根据勾选框内容设置RTS与DTR
        if(session_mgr!=NULL)
        {
            session_mgr->setRTS(ui->RTScheckBox->isChecked());
            session_mgr->setDTR(ui->DTRcheckBox->isChecked());
        }
    }

    if(session_mgr!=NULL)
    {
        //打印串口信息
        QSerialPortInfo info=session_mgr->getInfo();
        qDebug()<<info.portName()+" ("+info.description()+") "+tr("Opened");
        if(rightstatus!=NULL)
        {
            QString statusstr;
            statusstr+= info.portName()+" ("+info.description()+") "+tr("Opened");
            rightstatus->setText(statusstr);
        }

        //清空hex输出

        {
            auto list=HexWidgetList;;
            for(auto it=list.begin();it!=list.end();it++)
            {
                QWidget *widget=dynamic_cast<QWidget *>(*it);
                if(widget!=NULL)
                {
                    HexList->removeWidget(widget);
                    widget->deleteLater();
                }
            }
            HexWidgetList.clear();
        }

    }
}

void MainWindow::handleSessionClosed()
{
    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setDisabled(true);

    // disable file transfer and input line
    ui->fileTransferButton->setDisabled(true);
    ui->inputBox->setDisabled(true);

    //关闭RTS与DTR勾选框
    ui->RTScheckBox->setEnabled(false);
    ui->DTRcheckBox->setEnabled(false);

    if(rightstatus!=NULL)
    {
        QString statusstr;
        statusstr+=tr("Closed");
        rightstatus->setText(statusstr);
    }

}

void MainWindow::handleFileTransfer()
{
    QString filename = QFileDialog::getOpenFileName(
                this, QStringLiteral("Select file to send transfer"));

    if (filename.isNull())
        return;

    Q_ASSERT_X(progress_dialog == 0, "MainWindow::handleFileTransfer()", "progress_dialog should be null");

    // display a progress dialog
    progress_dialog = new QProgressDialog(this);
    connect(progress_dialog, &QProgressDialog::canceled,
            session_mgr, &SessionManager::handleTransferCancelledByUser);

    progress_dialog->setRange(0, 100);
    progress_dialog->setWindowModality(Qt::ApplicationModal);
    progress_dialog->setLabelText(
                QStringLiteral("Initiating connection with receiver"));

    // update progress dialog
    connect(session_mgr, &SessionManager::fileTransferProgressed,
            this, &MainWindow::handleFileTransferProgressed);

    int protocol = ui->protocolCombo->currentData().toInt();
    session_mgr->transferFile(filename,
        static_cast<SessionManager::Protocol>(protocol));

    // disable UI elements acting on QSerialPort instance, as long as
    // objectds involved in FileTransferred are not destroyed or back
    // to their pre-file-transfer state
    ui->fileTransferButton->setEnabled(false);
    ui->disconnectButton->setEnabled(false);
    ui->inputBox->setEnabled(false);

    // progress dialog event loop
    progress_dialog->exec();

    delete progress_dialog;
    progress_dialog = 0;
}

void MainWindow::handleFileTransferProgressed(int percent)
{
    if (progress_dialog)
    {
        progress_dialog->setValue(percent);
        progress_dialog->setLabelText(QStringLiteral("Transferring file"));
    }
}

void MainWindow::handleFileTransferEnded(FileTransfer::TransferError error)
{
    switch (error)
    {
        case FileTransfer::LocalCancelledError:
            break;
        case FileTransfer::NoError:
            QMessageBox::information(this, tr("Cutecom-ng"), QStringLiteral("File transferred successfully"));
            break;
        default:
            progress_dialog->setLabelText(FileTransfer::errorString(error));
            break;
    }

    // re-enable UI elements acting on QSerialPort instance
    ui->fileTransferButton->setEnabled(true);
    ui->disconnectButton->setEnabled(true);
    ui->inputBox->setEnabled(true);
}

void MainWindow::handleNewInput(QString entry)
{
    // if session is not open, this also keeps user input
    if (session_mgr->isSessionOpen())
    {
        QByteArray to_send(entry.toLocal8Bit());
        if(!ui->HexcheckBox->isChecked())
        {
            //不为HEX输出,添加结束符
            to_send.append(_end_of_line);
        }
        else
        {
            //为HEX输出
            to_send.clear();
            QString input=entry;
            input=input.toLower();
            //去除空格
            input.replace(QChar(' '),QString());
            //去除回车
            input.replace(QChar('\r'),QString());
            //去除换行
            input.replace(QChar('\n'),QString());

            if(input.isEmpty())
            {
                QMessageBox::warning(this,tr("Warning"),tr("Input is empty"));
                return;
            }

            bool IsHexString=true;
            for(int i=0;i<input.size();i++)
            {
                if(input.at(i).isNumber() || input.at(i).isLetter())
                {
                    if(input.at(i).isLetter())
                    {
                        QChar c=input.at(i).toLower();
                        if(c > QChar('f'))
                        {
                            IsHexString=false;
                        }
                    }
                }
                else
                {
                    IsHexString=false;
                }

            }
            if(!IsHexString)
            {
                QMessageBox::warning(this,tr("Warning"),tr("Input is not hex string"));
                return;
            }
            else
            {
                for(int i=0;i<input.size();i+=2)
                {
                    QString hex=QString::fromStdString(input.toStdString().substr(i,2));
                    to_send.append(hex.toInt(NULL,16));
                }
            }


        }
        session_mgr->sendToSerial(to_send);
        ui->inputBox->clearEditText();
    }
}

void MainWindow::addDataToView(const QString & textdata)
{
    // problem : QTextEdit interprets a '\r' as a new line, so if a buffer ending
    //           with '\r\n' happens to be cut in the middle, there will be 1 extra
    //           line jump in the QTextEdit. To prevent we remove ending '\r' and
    //           prepend them to the next received buffer

    // flag indicating that the previously received buffer ended with CR
    static bool prev_ends_with_CR = false;

    QString newdata;
    if (prev_ends_with_CR)
    {
        // CR was removed at the previous buffer, so now we prepend it
        newdata.append('\r');
        prev_ends_with_CR = false;
    }

    if (textdata.length() > 0)
    {
        QString::const_iterator end_cit = textdata.cend();
        if (textdata.endsWith('\r'))
        {
            // if buffer ends with CR, we don't copy it
            end_cit--;
            prev_ends_with_CR = true;
        }
        std::copy(textdata.begin(), end_cit, std::back_inserter(newdata));
    }

    // record end cursor position before adding text
    QTextCursor prev_end_cursor(ui->mainOutput->document());
    prev_end_cursor.movePosition(QTextCursor::End);

    if (ui->bottomOutput->isVisible())
    {
        // append text to the top output and stay at current position
        QTextCursor cursor(ui->mainOutput->document());
        cursor.movePosition(QTextCursor::End);
        cursor.insertText(newdata);
    }
    else
    {
        // append text to the top output and scroll down
        ui->mainOutput->moveCursor(QTextCursor::End);
        ui->mainOutput->insertPlainText(newdata);
    }

    // append text to bottom output and scroll
    ui->bottomOutput->moveCursor(QTextCursor::End);
    ui->bottomOutput->insertPlainText(newdata);
}

void MainWindow::handleDataReceived(const QByteArray &data)
{
    (*output_mgr) << data;

    //主输出
    if(!ui->actionmain_output->isChecked())
    {
        ui->mainOutput->clear();
    }

    //VT100 输出
    if(ui->actionvt100_output->isChecked())
    {
        ui->VT100Output->appendData(data);
    }

    //hex输出
    if(ui->actionhexoutput->isChecked())
    {
        HexListItem *item=new HexListItem(ui->hexOutputList);
        item->SetData(data);
        HexList->addWidget(item);
        HexWidgetList.append(item);
    }

}

void MainWindow::toggleOutputSplitter()
{
    ui->bottomOutput->setVisible(!ui->bottomOutput->isVisible());
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::Resize && target == ui->mainOutput->viewport())
    {
        // re position search widget when main output inner size changes
        // this takes into account existence of vertical scrollbar
        QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
        search_widget->move(resizeEvent->size().width() - search_widget->width(), 0);
    }
    else if (event->type() == QEvent::Wheel)
    {
        // mouse wheel allowed only for main output in 'browsing mode'
        if (target == ui->bottomOutput->viewport() ||
                (target == ui->mainOutput->viewport() && !ui->bottomOutput->isVisible()))
            return true;
    }
    else if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (ui->searchButton->isChecked())
        {
            // hide search widget on Escape key press
            if (keyEvent->key() == Qt::Key_Escape)
                ui->searchButton->toggle();
        }
        else
        {
            // show search widget on Ctrl-F
            if (keyEvent->key() == Qt::Key_F && keyEvent->modifiers() == Qt::ControlModifier)
                ui->searchButton->toggle();
        }
        if (target == search_input)
        {
            if (keyEvent->key() == Qt::Key_Return)
            {
                if (keyEvent->modifiers() == Qt::ShiftModifier)
                    search_prev_button->click();
                else
                    search_next_button->click();
            }
        }
    }

    // base class behaviour
    return QMainWindow::eventFilter(target, event);
}

void MainWindow::showSearchWidget(bool show)
{
    // record which widget had focus before showing search widget, in order
    // to return focus to it when search widget is hidden
    static QWidget *prevFocus = 0;

    QPropertyAnimation *animation = new QPropertyAnimation(search_widget, "pos");
    animation->setDuration(150);

    // place search widget at the rightmost position of mainoutput viewport
    QPoint showed_pos(ui->mainOutput->viewport()->width() - search_widget->width(), 0);
    QPoint hidden_pos(ui->mainOutput->viewport()->width() - search_widget->width(), -search_widget->height());
    animation->setStartValue(show ? hidden_pos : showed_pos);
    animation->setEndValue(show ? showed_pos : hidden_pos);

    if (show)
    {
        prevFocus = QApplication::focusWidget();
        search_widget->setVisible(show);
        search_input->setFocus();
    }
    else
    {
        connect(animation, &QPropertyAnimation::destroyed, search_widget, &QFrame::hide);
        prevFocus->setFocus();
    }
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::handleCursosPosChanged(int pos)
{
    // move cursor
    QTextCursor text_cursor = ui->mainOutput->textCursor();
    text_cursor.setPosition(pos);

    // ensure search result cursor is visible
    ui->mainOutput->ensureCursorVisible();
    ui->mainOutput->setTextCursor(text_cursor);
}

void MainWindow::handleTotalOccurencesChanged(int total_occurences)
{
    if (total_occurences == 0)
        search_input->setStyleSheet("QLineEdit{background-color: red;}");
    else
        search_input->setStyleSheet("");
}

void MainWindow::handleEOLCharChanged(int index)
{
    Q_UNUSED(index)

    switch(ui->eolCombo->currentData().toInt())
    {
        case CR:
            _end_of_line = QByteArray("\r", 1);
            break;
        case LF:
            _end_of_line = QByteArray("\n", 1);
            break;
        case CRLF:
            _end_of_line = QByteArray("\r\n", 2);
            break;
        case None:
            _end_of_line.clear();
            break;
        default:
            Q_ASSERT_X(false, "MainWindow::handleEOLCharChanged",
                       "unknown EOL char value: " + ui->eolCombo->currentData().toInt());
            break;
    }
}

void MainWindow::sessionManager_statisticChanged(uint64_t bytesRead,uint64_t bytesWrite)
{
    QString statistic=QString::fromStdString(std::string("R:")+std::to_string(bytesRead)+",S:"+std::to_string(bytesWrite));
    if(rightstatus_2!=NULL)
    {
        rightstatus_2->setText(statistic);
    }
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}


void MainWindow::on_action_Exit_triggered()
{
    close();
}


void MainWindow::on_actionEnglish_triggered()
{
    if(translator==NULL)
    {
        return;
    }
    translator->load(":/en_US.qm");
    ui->retranslateUi(this);
}


void MainWindow::on_actionChinese_triggered()
{
    if(translator==NULL)
    {
        return;
    }
    translator->load(":/zh_CN.qm");
    ui->retranslateUi(this);

}


void MainWindow::on_RTScheckBox_stateChanged(int)
{
    if(session_mgr!=NULL)
    {
        session_mgr->setRTS(ui->RTScheckBox->isChecked());
    }
}


void MainWindow::on_DTRcheckBox_stateChanged(int)
{
    if(session_mgr!=NULL)
    {
        session_mgr->setDTR(ui->DTRcheckBox->isChecked());
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    //刷新翻译
    ui->retranslateUi(this);
    qml_plugin_menu->setTitle(tr("qml plugin"));
    QMainWindow::paintEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //按键按下
    if(ui->actiondirentinput->isChecked())
    {
        QString input=event->text();
        if(!input.isEmpty())
        {
            if(session_mgr!=NULL)
            {
                if(session_mgr->isSessionOpen())
                {
                    setFocus();
                    session_mgr->sendToSerial(input.toUtf8());
                }
            }
        }
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::on_actionhexoutput_triggered(bool checked)
{
    ui->hexOutput->setVisible(checked);
    if(!checked)
    {
        //清空内容
        {
            auto list=HexWidgetList;
            for(auto it=list.begin();it!=list.end();it++)
            {
                QWidget *widget=dynamic_cast<QWidget *>(*it);
                if(widget!=NULL)
                {
                    HexList->removeWidget(widget);
                    widget->deleteLater();
                }
            }
            HexWidgetList.clear();
        }
    }

}


void MainWindow::on_clearButton_clicked()
{
    //清空hex内容
    {
        auto list=HexWidgetList;
        for(auto it=list.begin();it!=list.end();it++)
        {
            QWidget *widget=dynamic_cast<QWidget *>(*it);
            if(widget!=NULL)
            {
                HexList->removeWidget(widget);
                widget->deleteLater();
            }
        }
        HexWidgetList.clear();
    }

    //清空VT100内容
    ui->VT100Output->clear();
}


void MainWindow::on_actionLoadQml_triggered()
{
    QUrl qml_path=QFileDialog::getOpenFileUrl(this,tr("qml path"));
    if(!qml_path.isEmpty())
    {
        LoadQmlPlugin(qml_path);
    }

}

bool MainWindow::LoadQmlPlugin(QUrl qml_path,bool Show)
{
    bool ret=false;
    qml_list[qml_path]=QSharedPointer<QQmlLoader>(new QQmlLoader(this));
    qml_list[qml_path].data()->LoadQmlSource(qml_path);
    QQmlLoader *qmlloader=qml_list[qml_path].data();
    if(qmlloader->GetLoadStatus()!=QQuickWidget::Null)
    {
        //加载成功
        if(qmlloader->GetLoadStatus()!=QQuickWidget::Error)
        {
            //目前没有出现错误
            if(!qmlloader->GetPluginName().isEmpty())
            {
                if(qml_plugin_menu!=NULL)
                {
                    QAction *act=NULL;
                    auto submenulist=qml_plugin_menu->actions();
                    for(auto it = submenulist.begin();it!=submenulist.end();it++)
                    {
                        act=*it;
                        if(act->iconText()==qmlloader->GetPluginName())
                        {
                            break;
                        }
                        else
                        {
                            act=NULL;
                        }
                    }
                    if(act!=NULL)
                    {
                        //删除重复名字的子菜单
                        qDebug()<< QString("repeated pulgin");
                        qml_plugin_menu->removeAction(act);
                        {
                            //卸载之前的插件

                            for(auto it=qml_list.begin();it!=qml_list.end();it++)
                            {
                                if((it->data()->GetPluginName()==qmlloader->GetPluginName())&& (qmlloader!=it->data()))
                                {
                                    qml_list.erase(it);
                                    break;
                                }
                            }
                        }
                    }
                    auto cb=[=]()
                    {
                        auto it=qml_list.find(qml_path);
                        if(it!=qml_list.end())
                        {
                            qml_list[qml_path].data()->show();
                        }
                    };
                    qml_plugin_menu->addAction(qmlloader->GetPluginName(),cb);
                }
                if(Show)
                {
                    qmlloader->show();
                }
                ret=true;
            }
            else
            {
               //并非插件，删除加载的qml窗口
               qml_list.erase(qml_list.find(qml_path));
            }
        }
        else
        {
            //已出现错误,删除加载的qml窗口
            qml_list.erase(qml_list.find(qml_path));
        }
    }
    else
    {
        //删除加载的qml窗口
        qml_list.erase(qml_list.find(qml_path));
    }
    return ret;
}

bool MainWindow::UnloadQmlPlugin(QUrl qml_path)
{
    bool ret=false;
    auto it=qml_list.find(qml_path);
    if(it!=qml_list.end())
    {
        QQmlLoader *qmlloader=it->data();
        QAction *act=NULL;
        auto submenulist=qml_plugin_menu->actions();
        for(auto it = submenulist.begin();it!=submenulist.end();it++)
        {
            act=*it;
            if(act->iconText()==qmlloader->GetPluginName())
            {
                break;
            }
            else
            {
                act=NULL;
            }
        }
        if(act!=NULL)
        {
            //删除子菜单
            qml_plugin_menu->removeAction(act);
        }
        qml_list.erase(it);
        ret=true;
    }
    return ret;
}

SessionManager * MainWindow::GetSessionManager()
{
    return session_mgr;
}

void MainWindow::SetConfigPath(QString _path)
{
    configpath=_path;

    //加载配置目录下的xml文件
    load_configdoc();
}

QString MainWindow::GetConfigPath()
{
    return configpath;
}

QDomDocument& MainWindow::GetConfigDoc()
{
    return configdoc;
}

QDomElement MainWindow::GetConfigRootNode()
{
    return configdoc.firstChildElement("cutecom-ng");
}

void MainWindow::on_actionpluginmanager_triggered()
{
    PluginManager dlg(this);
    dlg.setModal(true);
    dlg.exec();
}


void MainWindow::on_actionmain_output_triggered()
{
    //修改主输出可见性
    ui->mainOutput->setVisible(ui->actionmain_output->isChecked());
    ui->searchButton->setEnabled(ui->actionmain_output->isChecked());
}


void MainWindow::on_actionvt100_output_triggered(bool checked)
{
    //修改VT100的可见性
    ui->VT100Output->setVisible(checked);
}


void MainWindow::on_actionversion_triggered()
{
    QMessageBox::information(this,tr("Version"),PROJECT_VERSION_STR);
}

