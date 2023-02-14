#ifndef SERIALPORTSTATUS_H
#define SERIALPORTSTATUS_H

#include <QWidget>

namespace Ui {
class SerialPortStatus;
}

class SerialPortStatus : public QWidget
{
    Q_OBJECT

public:
    explicit SerialPortStatus(QWidget *parent = nullptr);
    ~SerialPortStatus();

    void SetSendLength(uint64_t length);
    void SetReceiveLength(uint64_t length);
    void SetOpenStatus(QString status);
    void SetSpeedStatus(int speed);
    void SetBitsConfig(QString config);
    void SetFlowControl(QString flowcontrol);

private:
    Ui::SerialPortStatus *ui;
};

#endif // SERIALPORTSTATUS_H
