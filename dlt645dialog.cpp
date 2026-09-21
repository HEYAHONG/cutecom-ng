#include "dlt645dialog.h"
#include "ui_dlt645dialog.h"
#include "mainwindow.h"

dlt645dialog::dlt645dialog(MainWindow *parent)
    : QDialog(parent)
    , ui(new Ui::dlt645dialog)
    , m_mainwindow{parent}
    , session(new dlt645sessionmanager)
{
    ui->setupUi(this);

    /*
     * 关联关联窗口关闭
     */
    connect(this,&dlt645dialog::destroyed,session,&dlt645sessionmanager::CloseSession);

    if(parent == NULL)
    {
        deleteLater();
        return;
    }

    SessionManager *serialsession=parent->GetSessionManager();
    /*
     * 关联串口数据
     */
    connect(serialsession,&SessionManager::dataReceived,session,&dlt645sessionmanager::dataReceived);
    connect(session,&dlt645sessionmanager::sendToSerial,this,&dlt645dialog::sendToSerial);

}

void dlt645dialog::retranslateUi()
{
    loadconfig();
    ui->retranslateUi(this);
    saveconfig();
}

void dlt645dialog::loadconfig()
{

}

void dlt645dialog::saveconfig()
{

}

void dlt645dialog::sendToSerial(const QByteArray &data)
{
    if(m_mainwindow!=NULL)
    {
        SessionManager *serialsession=m_mainwindow->GetSessionManager();
        if(serialsession!=NULL)
        {
            if(serialsession->isSessionOpen())
            {
                serialsession->sendToSerial(data);
            }
        }
    }
}

dlt645dialog::~dlt645dialog()
{
    delete ui;
}
