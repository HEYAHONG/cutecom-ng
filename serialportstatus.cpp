#include "serialportstatus.h"
#include "ui_serialportstatus.h"

SerialPortStatus::SerialPortStatus(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialPortStatus)
{
    ui->setupUi(this);
}

void SerialPortStatus::SetSendLength(uint64_t length)
{
    ui->sendlength->setText(std::to_string(length).c_str());
}
void SerialPortStatus::SetReceiveLength(uint64_t length)
{
    ui->receivelength->setText(std::to_string(length).c_str());
}

void SerialPortStatus::SetOpenStatus(QString status)
{
    ui->openstatus->setText(status);
}

void SerialPortStatus::SetSpeedStatus(int speed)
{
    ui->speedstatus->setText(std::to_string(speed).c_str());
}

void SerialPortStatus::SetBitsConfig(QString config)
{
    ui->bitsconfig->setText(config);
}

void SerialPortStatus::SetFlowControl(QString flowcontrol)
{
   ui->flowcontrol->setText(flowcontrol);
}

SerialPortStatus::~SerialPortStatus()
{
    delete ui;
}
