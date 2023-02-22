#ifndef MODBUSRTUWRITEDATADIALOG_H
#define MODBUSRTUWRITEDATADIALOG_H

#include <QDialog>
#include <QStringListModel>
#include <QVector>

namespace Ui {
class ModbusRTUWriteDataDialog;
}

class ModbusRTUWriteDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModbusRTUWriteDataDialog(QWidget *parent = nullptr);
    ~ModbusRTUWriteDataDialog();

    bool IsReadAfterWrite();
    int GetStartAddr();
    QVector<uint16_t> GetData();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

    void on_AddButton_clicked();

    void on_DelButton_clicked();

    void on_OkButton_clicked();

private:
    Ui::ModbusRTUWriteDataDialog *ui;

    QStringListModel *db_list;

    QVector<uint16_t> data;

};

#endif // MODBUSRTUWRITEDATADIALOG_H
