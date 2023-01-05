#include "hexlistitem.h"
#include "ui_hexlistitem.h"

HexListItem::HexListItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HexListItem)
{
    ui->setupUi(this);

    //设定时间
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit->setReadOnly(true);
    ui->dateTimeEdit->setDisplayFormat("yyyy-MM-dd hh:mm:ss zzz");

    //设置hexoutput
    ui->hexOutput->setDynamicBytesPerLine(true);
    ui->hexOutput->setReadOnly(true);
    ui->hexOutput->setHexCaps(true);
}

void HexListItem::SetData(const QByteArray &data)
{
    ui->hexOutput->setData(data);
}

HexListItem::~HexListItem()
{
    delete ui;
}
