#ifndef MODBUSRTUDIALOG_H
#define MODBUSRTUDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QStandardItemModel>
#include "modbussessionmanager.h"
#include <QStringListModel>
class MainWindow;

namespace Ui {
class ModbusRTUDialog;
}

class ModbusRTUDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModbusRTUDialog(MainWindow *parent = nullptr);
    ~ModbusRTUDialog();

private:
    Ui::ModbusRTUDialog *ui;
    ModbusSessionManager *session;

    QStandardItemModel *db_Coils;
    QStandardItemModel *db_DiscreteInputs;
    QStandardItemModel *db_HoldingRegisters;
    QStandardItemModel *db_InputRegisters;
    QStringListModel *db_DataSend;

    void ClearAllOldTableData();
    void ReadCoils();
    void WriteCoils();
    void ReadDiscreteInputs();
    void ReadHoldingRegisters();
    void WriteHoldingRegisters();
    void ReadInputRegisters();


public slots:
    /*
     *串口数据接收,一般用于连接SessionManager
    */
    void dataReceived(const QByteArray &data);

    /*
     * 串口数据发送, 一般用于连接ModbusSessionManager
     */
    void SendSerialData(const QByteArray data);

    /*
     *Modbus会话数据改变
    */
    void SessionDataChanged();
private slots:
    void on_AddressFormat_currentTextChanged(const QString &arg1);
    void on_ValueFormat_currentTextChanged(const QString &arg1);
    void on_CoilsTable_customContextMenuRequested(const QPoint &pos);
    void on_DiscreteInputsTable_customContextMenuRequested(const QPoint &pos);
    void on_HoldingRegistersTable_customContextMenuRequested(const QPoint &pos);
    void on_InputRegistersTable_customContextMenuRequested(const QPoint &pos);
    void on_CoilsStartAddr_textChanged(const QString &arg1);
    void on_CoilsLength_textChanged(const QString &arg1);
    void on_DiscreteInputsStartAddr_textChanged(const QString &arg1);
    void on_DiscreteInputsLength_textChanged(const QString &arg1);
    void on_HoldingRegisterStartAddr_textChanged(const QString &arg1);
    void on_HoldingRegisterLength_textChanged(const QString &arg1);
    void on_InputRegisterStartAddr_textChanged(const QString &arg1);
    void on_InputRegisterLength_textChanged(const QString &arg1);
    void on_DataSendMaxLine_textChanged(const QString &arg1);

    void on_DataSendList_customContextMenuRequested(const QPoint &pos);

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // MODBUSRTUDIALOG_H
