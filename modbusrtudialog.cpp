#include "modbusrtudialog.h"
#include "ui_modbusrtudialog.h"
#include "QMenu"
#include "mainwindow.h"
#include "modbusrtuwritedatadialog.h"
#include <QClipboard>

ModbusRTUDialog::ModbusRTUDialog(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::ModbusRTUDialog),
    session(new ModbusSessionManager)
{
    ui->setupUi(this);


    connect(this,&ModbusRTUDialog::destroyed,session,&ModbusSessionManager::CloseSession);
    connect(session,&ModbusSessionManager::SendSerialData,this,&ModbusRTUDialog::SendSerialData);
    connect(session,&ModbusSessionManager::DataChanged,this,&ModbusRTUDialog::SessionDataChanged);
    if(parent!=NULL)
    {
       SessionManager *serialsession=parent->GetSessionManager();
       connect(serialsession,&SessionManager::dataReceived,this,&ModbusRTUDialog::dataReceived);
    }

    {
        //初始化表格
        db_Coils = new QStandardItemModel(ui->CoilsTable);
        ui->CoilsTable->setModel(db_Coils);
        db_DiscreteInputs = new QStandardItemModel(ui->DiscreteInputsTable);
        ui->DiscreteInputsTable->setModel(db_DiscreteInputs);
        db_HoldingRegisters = new QStandardItemModel(ui->HoldingRegistersTable);
        ui->HoldingRegistersTable->setModel(db_HoldingRegisters);
        db_InputRegisters = new QStandardItemModel(ui->InputRegistersTable);
        ui->InputRegistersTable->setModel(db_InputRegisters);
        ClearAllOldTableData();
    }

    {
        db_DataSend = new QStringListModel(ui->DataSendList);
        ui->DataSendList->setModel(db_DataSend);
        ui->DataSendList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->DataSendList->setContextMenuPolicy(Qt::CustomContextMenu);
    }

    {
        //初始化从机地址选择
        QStringList list;
        for(int i=0;i<248;i++)
        {
            list.append(std::to_string(i).c_str());
        }
        ui->SalveAddr->addItems(list);
        ui->SalveAddr->setCurrentText("1");
    }
    {
        QStringList list;
        list << QString("hex") << QString("dec");
        ui->AddressFormat->addItems(list);
        ui->ValueFormat->addItems(list);
        ui->AddressFormat->setCurrentText("hex");
        ui->ValueFormat->setCurrentText("hex");
    }
    {
        ui->CoilsStartAddr->setText("0");
        ui->CoilsLength->setText("1");
        ui->DiscreteInputsStartAddr->setText("0");
        ui->DiscreteInputsLength->setText("1");
        ui->HoldingRegisterStartAddr->setText("0");
        ui->HoldingRegisterLength->setText("1");
        ui->InputRegisterStartAddr->setText("0");
        ui->InputRegisterLength->setText("1");
    }

}

ModbusRTUDialog::~ModbusRTUDialog()
{
    delete ui;
}

 void ModbusRTUDialog::ClearAllOldTableData()
 {
    db_Coils->clear();
    db_DiscreteInputs->clear();
    db_HoldingRegisters->clear();
    db_InputRegisters->clear();

    //初始化标题
    QStringList table_header;
    table_header << tr("Address")<<tr("Value") << tr("Time");
    db_Coils->setHorizontalHeaderLabels(table_header);
    db_DiscreteInputs->setHorizontalHeaderLabels(table_header);
    db_HoldingRegisters->setHorizontalHeaderLabels(table_header);
    db_InputRegisters->setHorizontalHeaderLabels(table_header);

    //设置宽度及高度策略
    ui->CoilsTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->CoilsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->DiscreteInputsTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->DiscreteInputsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->HoldingRegistersTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->HoldingRegistersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->InputRegistersTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->InputRegistersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //设置菜单策略
    ui->CoilsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->DiscreteInputsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->HoldingRegistersTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->InputRegistersTable->setContextMenuPolicy(Qt::CustomContextMenu);

    //设置只读模式
    ui->CoilsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->DiscreteInputsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->HoldingRegistersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->InputRegistersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

 }

 void ModbusRTUDialog::SessionDataChanged()
 {
    //数据改变
    if(session!=NULL)
    {
        ClearAllOldTableData();
        {
            auto tb=session->GetAllCoils();
            int row=0;
            for(auto it=tb.begin();it!=tb.end();it++)
            {
                QString addr;
                {
                    uint16_t addrval=it.key();
                    if(ui->AddressFormat->currentText()=="hex")
                    {
                        char buff[12]={0};
                        sprintf(buff,"%04X",(int)addrval);
                        addr=buff;
                    }
                    else
                    {
                        char buff[12]={0};
                        sprintf(buff,"%04d",(int)addrval);
                        addr=buff;
                    }
                }

                QString value;
                {
                    uint16_t valueval=it.value().data;
                    if(ui->ValueFormat->currentText()=="hex")
                    {
                        char buff[12]={0};
                        sprintf(buff,"%04X",(int)valueval);
                        value=buff;
                    }
                    else
                    {
                        char buff[12]={0};
                        sprintf(buff,"%04d",(int)valueval);
                        value=buff;
                    }
                }

                QString time=it.value().time.toString(Qt::ISODate);

                db_Coils->setItem(row,0,new QStandardItem(addr));
                db_Coils->setItem(row,1,new QStandardItem(value));
                db_Coils->setItem(row,2,new QStandardItem(time));

                row++;
            }
        }

        {
            auto tb=session->GetAllDiscreteInputs();
            int row=0;
            for(auto it=tb.begin();it!=tb.end();it++)
            {
                QString addr;
                {
                    uint16_t addrval=it.key();
                    if(ui->AddressFormat->currentText()=="hex")
                    {
                        char buff[12]={0};
                        sprintf(buff,"%04X",(int)addrval);
                        addr=buff;
                    }
                    else
                    {
                        char buff[12]={0};
                        sprintf(buff,"%04d",(int)addrval);
                        addr=buff;
                    }
                }

                QString value;
                {
                    uint16_t valueval=it.value().data;
                    if(ui->ValueFormat->currentText()=="hex")
                    {
                        char buff[12]={0};
                        sprintf(buff,"%04X",(int)valueval);
                        value=buff;
                    }
                    else
                    {
                        char buff[12]={0};
                        sprintf(buff,"%04d",(int)valueval);
                        value=buff;
                    }
                }

                QString time=it.value().time.toString(Qt::ISODate);

                db_DiscreteInputs->setItem(row,0,new QStandardItem(addr));
                db_DiscreteInputs->setItem(row,1,new QStandardItem(value));
                db_DiscreteInputs->setItem(row,2,new QStandardItem(time));

                row++;
            }
        }

        {
            auto tb=session->GetAllHoldingRegisters();
            int row=0;
            for(auto it=tb.begin();it!=tb.end();it++)
            {
                QString addr;
                {
                    uint16_t addrval=it.key();
                    if(ui->AddressFormat->currentText()=="hex")
                    {
                        char buff[12]={0};
                        sprintf(buff,"%04X",(int)addrval);
                        addr=buff;
                    }
                    else
                    {
                        char buff[12]={0};
                        sprintf(buff,"%04d",(int)addrval);
                        addr=buff;
                    }
                }

                QString value;
                {
                    uint16_t valueval=it.value().data;
                    if(ui->ValueFormat->currentText()=="hex")
                    {
                        char buff[12]={0};
                        sprintf(buff,"%04X",(int)valueval);
                        value=buff;
                    }
                    else
                    {
                        char buff[12]={0};
                        sprintf(buff,"%04d",(int)valueval);
                        value=buff;
                    }
                }

                QString time=it.value().time.toString(Qt::ISODate);

                db_HoldingRegisters->setItem(row,0,new QStandardItem(addr));
                db_HoldingRegisters->setItem(row,1,new QStandardItem(value));
                db_HoldingRegisters->setItem(row,2,new QStandardItem(time));

                row++;
            }
        }

        {
            auto tb=session->GetAllInputRegisters();
            int row=0;
            for(auto it=tb.begin();it!=tb.end();it++)
            {
                QString addr;
                {
                    uint16_t addrval=it.key();
                    if(ui->AddressFormat->currentText()=="hex")
                    {
                        char buff[12]={0};
                        sprintf(buff,"%04X",(int)addrval);
                        addr=buff;
                    }
                    else
                    {
                        char buff[12]={0};
                        sprintf(buff,"%04d",(int)addrval);
                        addr=buff;
                    }
                }

                QString value;
                {
                    uint16_t valueval=it.value().data;
                    if(ui->ValueFormat->currentText()=="hex")
                    {
                        char buff[12]={0};
                        sprintf(buff,"%04X",(int)valueval);
                        value=buff;
                    }
                    else
                    {
                        char buff[12]={0};
                        sprintf(buff,"%04d",(int)valueval);
                        value=buff;
                    }
                }

                QString time=it.value().time.toString(Qt::ISODate);

                db_InputRegisters->setItem(row,0,new QStandardItem(addr));
                db_InputRegisters->setItem(row,1,new QStandardItem(value));
                db_InputRegisters->setItem(row,2,new QStandardItem(time));

                row++;
            }
        }
    }
 }

 void ModbusRTUDialog::ReadCoils()
 {
    uint8_t slaveaddr=ui->SalveAddr->currentText().toInt();
    size_t  length=ui->CoilsLength->text().toInt();
    QString startaddr_str=ui->CoilsStartAddr->text();
    uint16_t startaddr=startaddr_str.toInt(NULL,16);
    if(ui->AddressFormat->currentText()!="hex")
    {
        startaddr=startaddr_str.toInt();
    }
    if(session!=NULL)
    {
        session->RequestModbusRead(slaveaddr,ModbusSessionManager::ReadCoils,startaddr,length);
    }
 }
 void ModbusRTUDialog::WriteCoils()
 {
    uint8_t slaveaddr=ui->SalveAddr->currentText().toInt();
    ModbusRTUWriteDataDialog dlg;
    if(dlg.exec()==QDialog::Accepted)
    {
        if(session!=NULL)
        {
            session->RequestModbusWrite(slaveaddr,ModbusSessionManager::WriteCoils,dlg.GetStartAddr(),dlg.GetData());
        }
    }
 }
 void ModbusRTUDialog::ReadDiscreteInputs()
 {
    uint8_t slaveaddr=ui->SalveAddr->currentText().toInt();
    size_t  length=ui->DiscreteInputsLength->text().toInt();
    QString startaddr_str=ui->DiscreteInputsStartAddr->text();
    uint16_t startaddr=startaddr_str.toInt(NULL,16);
    if(ui->AddressFormat->currentText()!="hex")
    {
        startaddr=startaddr_str.toInt();
    }
    if(session!=NULL)
    {
        session->RequestModbusRead(slaveaddr,ModbusSessionManager::ReadDiscreteInputs,startaddr,length);
    }
 }
 void ModbusRTUDialog::ReadHoldingRegisters()
 {
    uint8_t slaveaddr=ui->SalveAddr->currentText().toInt();
    size_t  length=ui->HoldingRegisterLength->text().toInt();
    QString startaddr_str=ui->HoldingRegisterStartAddr->text();
    uint16_t startaddr=startaddr_str.toInt(NULL,16);
    if(ui->AddressFormat->currentText()!="hex")
    {
        startaddr=startaddr_str.toInt();
    }
    if(session!=NULL)
    {
        session->RequestModbusRead(slaveaddr,ModbusSessionManager::ReadHoldingRegisters,startaddr,length);
    }
 }
 void ModbusRTUDialog::WriteHoldingRegisters()
 {
    uint8_t slaveaddr=ui->SalveAddr->currentText().toInt();
    ModbusRTUWriteDataDialog dlg;
    if(dlg.exec()==QDialog::Accepted)
    {
        if(session!=NULL)
        {
            session->RequestModbusWrite(slaveaddr,ModbusSessionManager::WriteHoldingRegisters,dlg.GetStartAddr(),dlg.GetData());
        }
    }

 }
 void ModbusRTUDialog::ReadInputRegisters()
 {
    uint8_t slaveaddr=ui->SalveAddr->currentText().toInt();
    size_t  length=ui->InputRegisterLength->text().toInt();
    QString startaddr_str=ui->InputRegisterStartAddr->text();
    uint16_t startaddr=startaddr_str.toInt(NULL,16);
    if(ui->AddressFormat->currentText()!="hex")
    {
        startaddr=startaddr_str.toInt();
    }
    if(session!=NULL)
    {
        session->RequestModbusRead(slaveaddr,ModbusSessionManager::ReadInputRegisters,startaddr,length);
    }
 }


void ModbusRTUDialog::dataReceived(const QByteArray &data)
{
    if(session!=NULL)
    {
        session->SerialDataReceived(data);
    }
}

void ModbusRTUDialog::SendSerialData(const QByteArray data)
{
    MainWindow *mainwindow=dynamic_cast<MainWindow *>(parent());
    if(mainwindow!=NULL)
    {
        SessionManager *serialsession=mainwindow->GetSessionManager();
        if(serialsession!=NULL)
        {
            if(serialsession->isSessionOpen())
            {
                serialsession->sendToSerial(data);
            }
        }
    }

    if(db_DataSend!=NULL && !data.isEmpty())
    {
        QStringList list=db_DataSend->stringList();

        {
            QString datastr;
            std::string databyte=data.toStdString();
            for(auto it=databyte.begin();it!=databyte.end();it++)
            {
                char i=(*it);
                char buff[10]={0};
                sprintf(buff,"%02X ",(i & 0xFF));
                datastr+=QString(buff);
            }
            list.insert(0,datastr);
        }

        {
            int maxline = ui->DataSendMaxLine->text().toInt();
            if(maxline <= 0)
            {
                maxline=1000;
            }

            while(list.length()>maxline)
            {
                list.erase(list.begin()+(list.length()-1));
            }
        }

        db_DataSend->setStringList(list);
    }
}

void ModbusRTUDialog::on_AddressFormat_currentTextChanged(const QString &)
{
    ClearAllOldTableData();
    ui->CoilsStartAddr->setText("0");
    ui->DiscreteInputsStartAddr->setText("0");
    ui->HoldingRegisterStartAddr->setText("0");
    ui->InputRegisterStartAddr->setText("0");
}


void ModbusRTUDialog::on_ValueFormat_currentTextChanged(const QString &)
{
    ClearAllOldTableData();
}


void ModbusRTUDialog::on_CoilsTable_customContextMenuRequested(const QPoint &)
{
    QMenu *menu=new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction(tr("ReadCoils"),[=](){ReadCoils();});
    menu->addSeparator();
    menu->addAction(tr("WriteCoils"),[=](){WriteCoils();});
    menu->exec(QCursor::pos());
}


void ModbusRTUDialog::on_DiscreteInputsTable_customContextMenuRequested(const QPoint &)
{
    QMenu *menu=new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction(tr("ReadDiscreteInputs"),[=](){ReadDiscreteInputs();});
    menu->addSeparator();
    menu->exec(QCursor::pos());
}


void ModbusRTUDialog::on_HoldingRegistersTable_customContextMenuRequested(const QPoint &)
{
    QMenu *menu=new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction(tr("ReadHoldingRegisters"),[=](){ReadHoldingRegisters();});
    menu->addSeparator();
    menu->addAction(tr("WriteHoldingRegisters"),[=](){WriteHoldingRegisters();});
    menu->exec(QCursor::pos());
}


void ModbusRTUDialog::on_InputRegistersTable_customContextMenuRequested(const QPoint &)
{
    QMenu *menu=new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction(tr("ReadInputRegisters"),[=](){ReadInputRegisters();});
    menu->addSeparator();
    menu->exec(QCursor::pos());
}


void ModbusRTUDialog::on_CoilsStartAddr_textChanged(const QString &arg1)
{

    bool isok=false;
    int val=-1;
    bool ishex=(ui->AddressFormat->currentText()=="hex");
    if(ishex)
    {
        val=arg1.toInt(&isok,16);
    }
    bool isdec=(ui->AddressFormat->currentText()=="dec");
    if(isdec)
    {
        val=arg1.toInt(&isok,10);
    }

    if(arg1.isEmpty())
    {
        isok=false;
    }

    if(!isok || val < 0 || val > 0xFFFF )
    {
        ui->CoilsStartAddr->undo();
    }
}


void ModbusRTUDialog::on_CoilsLength_textChanged(const QString &arg1)
{
    bool isok=false;
    int val=-1;
    {
        val=arg1.toInt(&isok,10);
    }
    if(arg1.isEmpty())
    {
        isok=false;
    }
    if(!isok || val < 1)
    {
        ui->CoilsLength->undo();
    }
}


void ModbusRTUDialog::on_DiscreteInputsStartAddr_textChanged(const QString &arg1)
{
    bool isok=false;
    int val=-1;
    bool ishex=(ui->AddressFormat->currentText()=="hex");
    if(ishex)
    {
        val=arg1.toInt(&isok,16);
    }
    bool isdec=(ui->AddressFormat->currentText()=="dec");
    if(isdec)
    {
        val=arg1.toInt(&isok,10);
    }
    if(arg1.isEmpty())
    {
        isok=false;
    }
    if(!isok || val < 0 || val > 0xFFFF)
    {
        ui->DiscreteInputsStartAddr->undo();
    }
}


void ModbusRTUDialog::on_DiscreteInputsLength_textChanged(const QString &arg1)
{
    bool isok=false;
    int val=-1;
    {
        val=arg1.toInt(&isok,10);
    }
    if(arg1.isEmpty())
    {
        isok=false;
    }
    if(!isok || val < 1)
    {
        ui->DiscreteInputsLength->undo();
    }
}


void ModbusRTUDialog::on_HoldingRegisterStartAddr_textChanged(const QString &arg1)
{
    bool isok=false;
    int val=-1;
    bool ishex=(ui->AddressFormat->currentText()=="hex");
    if(ishex)
    {
        val=arg1.toInt(&isok,16);
    }
    bool isdec=(ui->AddressFormat->currentText()=="dec");
    if(isdec)
    {
        val=arg1.toInt(&isok,10);
    }
    if(arg1.isEmpty())
    {
        isok=false;
    }
    if(!isok || val < 0 || val > 0xFFFF)
    {
        ui->HoldingRegisterStartAddr->undo();
    }
}


void ModbusRTUDialog::on_HoldingRegisterLength_textChanged(const QString &arg1)
{
    bool isok=false;
    int val=-1;
    {
        val=arg1.toInt(&isok,10);
    }
    if(arg1.isEmpty())
    {
        isok=false;
    }
    if(!isok || val < 1)
    {
        ui->HoldingRegisterLength->undo();
    }
}


void ModbusRTUDialog::on_InputRegisterStartAddr_textChanged(const QString &arg1)
{
    bool isok=false;
    int val=-1;
    bool ishex=(ui->AddressFormat->currentText()=="hex");
    if(ishex)
    {
        val=arg1.toInt(&isok,16);
    }
    bool isdec=(ui->AddressFormat->currentText()=="dec");
    if(isdec)
    {
        val=arg1.toInt(&isok,10);
    }
    if(arg1.isEmpty())
    {
        isok=false;
    }
    if(!isok || val < 0 || val > 0xFFFF)
    {
        ui->InputRegisterStartAddr->undo();
    }
}


void ModbusRTUDialog::on_InputRegisterLength_textChanged(const QString &arg1)
{
    bool isok=false;
    int val=-1;
    {
        val=arg1.toInt(&isok,10);
    }
    if(arg1.isEmpty())
    {
        isok=false;
    }
    if(!isok || val < 1)
    {
        ui->InputRegisterLength->undo();
    }
}

void ModbusRTUDialog::paintEvent(QPaintEvent *event)
{
    //ui->retranslateUi(this);
    QDialog::paintEvent(event);
}


void ModbusRTUDialog::on_DataSendMaxLine_textChanged(const QString &arg1)
{
    bool isok=false;
    int val=-1;
    {
        val=arg1.toInt(&isok,10);
    }
    if(arg1.isEmpty())
    {
        isok=false;
    }
    if(!isok || val < 1)
    {
        ui->DataSendMaxLine->undo();
    }
}


void ModbusRTUDialog::on_DataSendList_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    QMenu *menu=new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    {
        auto copy=[=]()
        {
            QStringList list=db_DataSend->stringList();
            if(list.isEmpty())
            {
                return;
            }
            if(ui->DataSendList->currentIndex().isValid())
            {
                int row=ui->DataSendList->currentIndex().row();
                if(row>=0)
                {
                    QGuiApplication::clipboard()->setText(list.at(row));
                }
            }
        };
        QAction * actcopy=menu->addAction(tr("Copy"),copy);
        if(!ui->DataSendList->currentIndex().isValid())
        {
            actcopy->setDisabled(true);
        }
    }
    menu->addSeparator();
    menu->exec(QCursor::pos());
}

