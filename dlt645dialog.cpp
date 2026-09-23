#include "dlt645dialog.h"
#include "ui_dlt645dialog.h"
#include "mainwindow.h"
#include <QCheckBox>

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

    connect(this,&dlt645dialog::Log,this,&dlt645dialog::Log_Slot);
    connect(this,&dlt645dialog::Read_Result,this,&dlt645dialog::Read_Result_Solt);

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
    connect(serialsession,&SessionManager::dataReceived,this,&dlt645dialog::dataReceived);
    connect(session,&dlt645sessionmanager::sendToSerial,this,&dlt645dialog::sendToSerial);

    /*
     * 关联状态改变
     */
    connect(session,&dlt645sessionmanager::StatusChanged,this,&dlt645dialog::SessionStatusChanged);

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

void dlt645dialog::log(QString log)
{
    emit Log(log);
}

void dlt645dialog::read_result(hdlt645_data_di_t di,const uint8_t *data,size_t datalen)
{
    if(data==NULL || datalen == 0)
    {
        return;
    }
    emit Read_Result(di,QByteArray((const char *)data,datalen));
}

void dlt645dialog::dataReceived(const QByteArray &data)
{
    if(session!=NULL)
    {
        if(session->dlt645_session_idle())
        {
            return;
        }
        char buffer[4096+1]= {0};
        if(sizeof(buffer)/2 >= data.length())
        {
            hbase16_encode_with_null_terminator(buffer,sizeof(buffer),(const uint8_t *)data.data(),data.length());
        }
        QString log_tx=QString("Rx %1").arg(buffer);
        log(log_tx);
    }

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

        if(session!=NULL)
        {
            char buffer[4096+1]= {0};
            if(sizeof(buffer)/2 >= data.length())
            {
                hbase16_encode_with_null_terminator(buffer,sizeof(buffer),(const uint8_t *)data.data(),data.length());
            }
            QString log_tx=QString("Tx %1").arg(buffer);
            log(log_tx);
        }
    }
}

void dlt645dialog::SessionStatusChanged()
{
    if(session != NULL)
    {
        if(session->dlt645_session_idle())
        {
            setEnabled(true);
        }
        else
        {
            setEnabled(false);
        }

        hdlt645_master_ctx_status_t status=session->dlt645_status();
        switch(status)
        {
        case HDLT645_MASTER_CTX_STATUS_ERROR:
        {
            log("Error!");
        }
        break;
        case HDLT645_MASTER_CTX_STATUS_FINISHED:
        {
            log("Finished!");
        }
        break;
        case HDLT645_MASTER_CTX_STATUS_INIT:
        {
            log("Init!");
        }
        break;
        case HDLT645_MASTER_CTX_STATUS_SEND_REQUEST:
        {
            log("Request Send!!");
        }
        break;
        case HDLT645_MASTER_CTX_STATUS_WAIT_REPLY:
        {
            log("Wait Reply!");
        }
        break;
        default:
        {

        }
        break;
        }
    }
}

dlt645dialog::~dlt645dialog()
{
    delete ui;
}




void dlt645dialog::on_Time_checkBox_stateChanged(int arg1)
{
    if(ui->Time_checkBox->checkState()!= Qt::Checked)
    {
        ui->Time_dateTimeEdit->setEnabled(true);
    }
    else
    {
        ui->Time_dateTimeEdit->setEnabled(false);
    }
}


void dlt645dialog::on_Time_pushButton_clicked()
{
    memset(&cmd_time,0,sizeof(cmd_time));
    if(ui->Time_checkBox->checkState()!= Qt::Checked)
    {
        cmd_time.usr=ui->Time_dateTimeEdit->dateTime().toSecsSinceEpoch();
        cmd_time.cb=[](hdlt645_master_ctx_cmd_time_t *cmd) -> htime_t
        {
            if(cmd!=NULL)
            {
                return (htime_t)cmd->usr;
            }
            return 0;
        };
    }
    if(session!=NULL && session->dlt645_session_idle())
    {
        session->dlt645_start_session(HDLT645_FRAME_CONTROL_FCT_TIME,&cmd_time,sizeof(cmd_time));
    }
}


void dlt645dialog::on_Time_pushButton_clicked(bool checked)
{
    on_Time_pushButton_clicked();
}

void dlt645dialog::Log_Slot(QString log)
{
    QString time_log=QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    QString log_text=QString("[ %1 ] %2").arg(time_log).arg(log);
    ui->TextEdit_Log->appendPlainText(log_text);
}


void dlt645dialog::on_Read_N_checkBox_stateChanged(int arg1)
{
    if(ui->Read_N_checkBox->checkState()==Qt::Checked)
    {
        ui->Read_N_lineEdit->setEnabled(true);
        ui->Read_Time_checkBox->setEnabled(true);
    }
    else
    {
        ui->Read_Time_checkBox->setCheckState(Qt::Unchecked);
        ui->Read_N_lineEdit->setEnabled(false);
        ui->Read_Time_checkBox->setEnabled(false);
    }
}


void dlt645dialog::on_Read_Time_checkBox_stateChanged(int arg1)
{
    if(ui->Read_Time_checkBox->checkState()==Qt::Checked)
    {
        ui->Read_Time_dateTimeEdit->setEnabled(true);
    }
    else
    {
        ui->Read_Time_dateTimeEdit->setEnabled(false);
    }

}

void dlt645dialog::Read_Result_Solt(hdlt645_data_di_t di,QByteArray data)
{

    size_t datalen=data.length();
    char buffer[4096+1]= {0};
    if(datalen > sizeof(buffer)/2)
    {
        datalen = sizeof(buffer)/2;
    }
    hbase16_encode_with_null_terminator(buffer,sizeof(buffer),(const uint8_t *)data.data(),datalen);
    {
        char di_buffer[16]= {0};
        hbase16_encode_with_null_terminator(di_buffer,sizeof(di_buffer),di.di,sizeof(di.di));
        log(QString("Read %1 %2").arg(di_buffer).arg(buffer));
    }
    ui->Read_Result_textEdit->setText(buffer);
    {
        const hdlt645_di_data_desc_t *desc=hdlt645_di_data_desc_get(&di);
        if(desc!=NULL)
        {
            switch(desc->type)
            {
            case HDLT645_DI_DATA_DESC_TYPE_XXXXXX_XX:
            case HDLT645_DI_DATA_DESC_TYPE_XXXXXX_XX_SIGNED:
            case HDLT645_DI_DATA_DESC_TYPE_XXX_X:
            case HDLT645_DI_DATA_DESC_TYPE_XXX_X_SIGNED:
            case HDLT645_DI_DATA_DESC_TYPE_XX_XX:
            case HDLT645_DI_DATA_DESC_TYPE_XX_XX_SIGNED:
            case HDLT645_DI_DATA_DESC_TYPE_X_XXX:
            case HDLT645_DI_DATA_DESC_TYPE_X_XXX_SIGNED:
            case HDLT645_DI_DATA_DESC_TYPE_XXX_XXX:
            case HDLT645_DI_DATA_DESC_TYPE_XXX_XXX_SIGNED:
            case HDLT645_DI_DATA_DESC_TYPE_XX_XXXX:
            case HDLT645_DI_DATA_DESC_TYPE_XX_XXXX_SIGNED:
            case HDLT645_DI_DATA_DESC_TYPE_XXXXXXXX:
            case HDLT645_DI_DATA_DESC_TYPE_XXXXXXXX_SIGNED:
            {
                double value=0;
                desc->get_data(desc,(const uint8_t *)data.data(),data.length(),&value);
                QString value_string=QString("Value: %1").arg(QString::number(value));
                ui->Read_Result_textEdit->append(value_string);
            }
            break;
            case HDLT645_DI_DATA_DESC_TYPE_XX_XXXX_YYMMDDHHMM:
            case HDLT645_DI_DATA_DESC_TYPE_XX_XXXX_YYMMDDHHMM_SIGNED:
            {
                hdlt645_di_data_desc_type_xx_xxxx_yymmddhhmm_signed_t value;
                memset(&value,0,sizeof(value));
                desc->get_data(desc,(const uint8_t *)data.data(),data.length(),&value);
                QString value_string=QString("Value: %1 %2-%3-%4 %5:%6").arg(QString::number(value.val)).arg(QString::number(value.YY)).arg(QString::number(value.MM)).arg(QString::number(value.DD)).arg(QString::number(value.HH)).arg(QString::number(value.mm));
                ui->Read_Result_textEdit->append(value_string);
            }
            break;
            default:
            {

            }
            break;
            }
        }
    }
}


void dlt645dialog::on_Read_pushButton_clicked(bool checked)
{
    if(session==NULL || !session->dlt645_session_idle())
    {
        return;
    }
    bool is_ok=false;
    uint64_t addr_num=ui->Read_Addr_lineEdit->text().toLongLong(&is_ok,16);
    uint64_t di_num=ui->Read_DI_lineEdit->text().toLongLong(&is_ok,16);
    uint64_t n=ui->Read_N_lineEdit->text().toLongLong(&is_ok,16);
    htime_t  read_time=ui->Read_Time_dateTimeEdit->dateTime().toSecsSinceEpoch();

    if(ui->Read_Time_checkBox->checkState()==Qt::Checked)
    {
        hdlt645_data_di_t di;
        hdlt645_data_di_set(&di,di_num);
        hdlt645_bcd_addr_t addr;
        hdlt645_bcd_addr_set(&addr,addr_num);
        hdlt645_master_ctx_cmd_read_init3(&cmd_read,
                                          &addr,
                                          &di,
                                          n,
                                          read_time,
                                          [](hdlt645_master_ctx_cmd_read_t *cmd,hdlt645_data_di_t *di,const uint8_t *data,size_t datalen)
        {
            if(cmd==NULL || cmd->usr == 0 )
            {
                return;
            }
            dlt645dialog &obj=*(dlt645dialog *)cmd->usr;
            if(data == NULL || datalen == 0)
            {
                obj.log(QString("Read Empty!"));
                return;
            }
            emit obj.read_result(*di,data,datalen);
        },
        [](hdlt645_master_ctx_cmd_read_t *cmd,uint8_t err)
        {
            if(cmd==NULL || cmd->usr == 0)
            {
                return;
            }
            dlt645dialog &obj=*(dlt645dialog *)cmd->usr;
            obj.log(QString("Read Error %1").arg(QString::number(err,16)));

        },this);
    }
    else if(ui->Read_N_checkBox->checkState()==Qt::Checked)
    {
        hdlt645_data_di_t di;
        hdlt645_data_di_set(&di,di_num);
        hdlt645_bcd_addr_t addr;
        hdlt645_bcd_addr_set(&addr,addr_num);
        hdlt645_master_ctx_cmd_read_init2(&cmd_read,
                                          &addr,
                                          &di,
                                          n,
                                          [](hdlt645_master_ctx_cmd_read_t *cmd,hdlt645_data_di_t *di,const uint8_t *data,size_t datalen)
        {
            if(cmd==NULL || cmd->usr == 0 )
            {
                return;
            }
            dlt645dialog &obj=*(dlt645dialog *)cmd->usr;
            if(data == NULL || datalen == 0)
            {
                obj.log(QString("Read Empty!"));
                return;
            }
            emit obj.read_result(*di,data,datalen);
        },
        [](hdlt645_master_ctx_cmd_read_t *cmd,uint8_t err)
        {
            if(cmd==NULL || cmd->usr == 0)
            {
                return;
            }
            dlt645dialog &obj=*(dlt645dialog *)cmd->usr;
            obj.log(QString("Read Error %1").arg(QString::number(err,16)));

        },this);
    }
    else
    {
        hdlt645_data_di_t di;
        hdlt645_data_di_set(&di,di_num);
        hdlt645_bcd_addr_t addr;
        hdlt645_bcd_addr_set(&addr,addr_num);
        hdlt645_master_ctx_cmd_read_init1(&cmd_read,
                                          &addr,
                                          &di,
                                          [](hdlt645_master_ctx_cmd_read_t *cmd,hdlt645_data_di_t *di,const uint8_t *data,size_t datalen)
        {
            if(cmd==NULL || cmd->usr == 0 )
            {
                return;
            }
            dlt645dialog &obj=*(dlt645dialog *)cmd->usr;
            if(data == NULL || datalen == 0)
            {
                obj.log(QString("Read Empty!"));
                return;
            }
            emit obj.read_result(*di,data,datalen);
        },
        [](hdlt645_master_ctx_cmd_read_t *cmd,uint8_t err)
        {
            if(cmd==NULL || cmd->usr == 0)
            {
                return;
            }
            dlt645dialog &obj=*(dlt645dialog *)cmd->usr;
            obj.log(QString("Read Error %1").arg(QString::number(err,16)));

        },this);

    }

    if(session!=NULL && session->dlt645_session_idle())
    {
        session->dlt645_start_session(HDLT645_FRAME_CONTROL_FCT_READ,&cmd_read,sizeof(cmd_read));
    }


}


void dlt645dialog::on_Write_pushButton_clicked(bool checked)
{
    if(session==NULL || !session->dlt645_session_idle())
    {
        return;
    }
    bool is_ok=false;
    uint64_t addr_num=ui->Write_Addr_lineEdit->text().toLongLong(&is_ok,16);
    uint64_t di_num=ui->Write_DI_lineEdit->text().toLongLong(&is_ok,16);
    uint64_t p_num=ui->Write_P_lineEdit->text().toLongLong(&is_ok,16);
    uint64_t c_num=ui->Write_C_lineEdit->text().toLongLong(&is_ok,16);
    memset(&cmd_write,0,sizeof(cmd_write));
    memset(cmd_write_buffer,0,sizeof(cmd_write_buffer));
    size_t data_len=0;
    QByteArray data_text(ui->Write_Data_lineEdit->text().toUtf8());
    if(ui->Write_Data_checkBox->checkState() != Qt::Checked)
    {
        data_len=hbase16_decode(cmd_write_buffer,sizeof(cmd_write_buffer),data_text.data(),data_text.length());
    }
    else
    {
        if(data_text.length() > sizeof(cmd_write_buffer))
        {
            data_len=sizeof(cmd_write_buffer);
        }
        else
        {
            data_len=data_text.length();
        }
        memcpy(cmd_write_buffer,data_text.data(),data_len);
    }

    {
        hdlt645_bcd_addr_t addr;
        hdlt645_bcd_addr_set(&addr,addr_num);
        hdlt645_data_di_t di;
        hdlt645_data_di_set(&di,di_num);
        hdlt645_data_p_t p;
        hdlt645_data_p_set(&p,p_num);
        hdlt645_data_c_t c;
        hdlt645_data_c_set(&c,c_num);
        hdlt645_master_ctx_cmd_write_init(&cmd_write,
                                          &addr,
                                          &di,
                                          &p,
                                          &c,
                                          cmd_write_buffer,
                                          data_len,
                                          [](hdlt645_master_ctx_cmd_write_t *cmd,uint8_t err)
        {
            if(cmd==NULL || cmd->usr == 0)
            {
                return;
            }
            dlt645dialog &obj=*(dlt645dialog *)cmd->usr;
            obj.log(QString("Write Error %1").arg(QString::number(err,16)));
        },this);
    }

    if(session!=NULL && session->dlt645_session_idle())
    {
        session->dlt645_start_session(HDLT645_FRAME_CONTROL_FCT_WRITE,&cmd_write,sizeof(cmd_write));
    }

}

