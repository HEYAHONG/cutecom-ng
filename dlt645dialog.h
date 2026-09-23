#ifndef DLT645DIALOG_H
#define DLT645DIALOG_H

#include "HCPPBox.h"
#include <QDialog>
#include <dlt645sessionmanager.h>

class MainWindow;

namespace Ui
{
class dlt645dialog;
}

class dlt645dialog : public QDialog
{
    Q_OBJECT
    MainWindow *m_mainwindow;
    dlt645sessionmanager *session;
    hdlt645_master_ctx_cmd_time_t cmd_time;
    hdlt645_master_ctx_cmd_read_t cmd_read;
    hdlt645_master_ctx_cmd_write_t cmd_write;
    uint8_t cmd_write_buffer[HDLT645_FRAME_DATALENGTH_MAX_WRITE];
public:
    explicit dlt645dialog(MainWindow *parent = nullptr);
    ~dlt645dialog();

    void retranslateUi();

    /*
     *配置文件相关
     */
    void loadconfig();
    void saveconfig();

    void log(QString log);

signals:

    void Log(QString log);
    void Read_Result(hdlt645_data_di_t di,QByteArray data);

private:
    Ui::dlt645dialog *ui;
    void read_result(hdlt645_data_di_t di,const uint8_t *data,size_t datalen);
public slots:
    void dataReceived(const QByteArray &data);
    void sendToSerial(const QByteArray &data);
    void SessionStatusChanged();
private slots:
    void on_Time_checkBox_stateChanged(int arg1);
    void on_Time_pushButton_clicked();
    void on_Time_pushButton_clicked(bool checked);
    void Log_Slot(QString log);
    void on_Read_N_checkBox_stateChanged(int arg1);
    void on_Read_Time_checkBox_stateChanged(int arg1);
    void Read_Result_Solt(hdlt645_data_di_t di,QByteArray data);
    void on_Read_pushButton_clicked(bool checked);
    void on_Write_pushButton_clicked(bool checked);
};

#endif // DLT645DIALOG_H
