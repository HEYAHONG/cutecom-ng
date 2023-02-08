/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief Serial port connection and settings dialog
 *
 * \author Aurelien Rainone <aurelien@develer.com>
 */

#include "connectdialog.h"
#include "mainwindow.h"
#include "ui_connectdialog.h"

#include <QList>
#include <QHash>
#include <QtSerialPort>
#include <QSerialPortInfo>


ConnectDialog::ConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectDialog),
    mainwindow(dynamic_cast<MainWindow *>(parent))
{
    ui->setupUi(this);

    // fill the combo box values
    fillSettingsLists();

    // for device take 1st device listed (if any)
    QString default_device;
    if (ui->deviceList->count() > 0)
        default_device = ui->deviceList->itemText(0);

    // define a default configuration
    QHash<QString, QString> default_cfg;
    default_cfg[QStringLiteral("device")] = default_device;
    default_cfg[QStringLiteral("baud_rate")] = QString::number(QSerialPort::Baud115200);
    default_cfg[QStringLiteral("data_bits")] = QString::number(QSerialPort::Data8);
    default_cfg[QStringLiteral("stop_bits")] = QStringLiteral("1");
    default_cfg[QStringLiteral("parity")] = QStringLiteral("None");
    default_cfg[QStringLiteral("flow_control")] = QStringLiteral("None");

    // define the default values for output dump
    default_cfg[QStringLiteral("dump_enabled")] = QString::number(0);
    default_cfg[QStringLiteral("dump_file")] = QStringLiteral("cutecom-ng.dump");
    default_cfg[QStringLiteral("dump_format")] = QString::number(Raw);


    preselectPortConfig(default_cfg);


}

ConnectDialog::~ConnectDialog()
{
    delete ui;
}

void ConnectDialog::fillSettingsLists()
{
    // fill devices combo box
    QList<QSerialPortInfo> ports(QSerialPortInfo::availablePorts());
    for (int idx = 0; idx < ports.length(); ++idx)
    {
        const QSerialPortInfo& port_info = ports.at(idx);
        ui->deviceList->addItem(port_info.systemLocation());

        // construct description tooltip
        QString tooltip;

        // add decription if not empty
        if (!port_info.description().isEmpty())
            tooltip.append(port_info.description());
        if (!port_info.manufacturer().isEmpty())
        {
            // add ' / manufacturer' if not empty
            if (!tooltip.isEmpty())
                tooltip.push_back(QStringLiteral(" / "));
            tooltip.append(port_info.manufacturer());
        }
        // assign portName
        if (tooltip.isEmpty())
            tooltip = port_info.portName();

        ui->deviceList->setItemData(idx, tooltip, Qt::ToolTipRole);
    }

    // fill baud rates combo box
    QStringList baud_rates;
    baud_rates <<
        QString::number(QSerialPort::Baud1200) << QString::number(QSerialPort::Baud2400);
    baud_rates <<
        QString::number(QSerialPort::Baud4800) << QString::number(QSerialPort::Baud9600);
    baud_rates <<
        QString::number(QSerialPort::Baud19200) << QString::number(QSerialPort::Baud38400);
    baud_rates <<
        QString::number(QSerialPort::Baud57600) << QString::number(QSerialPort::Baud115200);
    ui->baudRateList->addItems(baud_rates);

    // fill data bits combo box
    QStringList data_bits;
    data_bits <<
        QString::number(QSerialPort::Data5) << QString::number(QSerialPort::Data6);
    data_bits <<
        QString::number(QSerialPort::Data7) << QString::number(QSerialPort::Data8);
    ui->dataBitsList->addItems(data_bits);

    // fill stop bits combo box
    ui->stopBitsList->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_WS_WIN
    ui->stopBitsList->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsList->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    // fill parity combo box
    ui->parityList->addItem(QStringLiteral("None"), QSerialPort::NoParity);
    ui->parityList->addItem(QStringLiteral("Even"), QSerialPort::EvenParity);
    ui->parityList->addItem(QStringLiteral("Odd"), QSerialPort::OddParity);
    ui->parityList->addItem(QStringLiteral("Space"), QSerialPort::SpaceParity);
    ui->parityList->addItem(QStringLiteral("Mark"), QSerialPort::MarkParity);

    // fill flow control
    ui->flowControlList->addItem(QStringLiteral("None"), QSerialPort::NoFlowControl);
    ui->flowControlList->addItem(QStringLiteral("Hardware"), QSerialPort::HardwareControl);
    ui->flowControlList->addItem(QStringLiteral("Software"), QSerialPort::SoftwareControl);
}

void ConnectDialog::preselectPortConfig(const QHash<QString, QString>& settings)
{
    ui->deviceList->setCurrentText(settings[QStringLiteral("device")]);
    ui->baudRateList->setCurrentText(settings[QStringLiteral("baud_rate")]);
    ui->dataBitsList->setCurrentText(settings[QStringLiteral("data_bits")]);
    ui->stopBitsList->setCurrentText(settings[QStringLiteral("stop_bits")]);

    ui->parityList->setCurrentText(settings[QStringLiteral("parity")]);
    ui->flowControlList->setCurrentText(settings[QStringLiteral("flow_control")]);

    ui->dumpFile->setChecked(settings[QStringLiteral("dump_enabled")] == "1");
    ui->dumpPath->setText(settings[QStringLiteral("dump_file")]);
    ui->dumpRawFmt->setChecked(settings["dump_format"] == QString::number(Raw));
    ui->dumpTextFmt->setChecked(settings["dump_format"] == QString::number(Ascii));
}

void ConnectDialog::loadconfig()
{
    // for device take 1st device listed (if any)
    QString default_device;
    if (ui->deviceList->count() > 0)
        default_device = ui->deviceList->itemText(0);

    // define a default configuration
    QHash<QString, QString> default_cfg;
    default_cfg[QStringLiteral("device")] = default_device;
    default_cfg[QStringLiteral("baud_rate")] = QString::number(QSerialPort::Baud115200);
    default_cfg[QStringLiteral("data_bits")] = QString::number(QSerialPort::Data8);
    default_cfg[QStringLiteral("stop_bits")] = QStringLiteral("1");
    default_cfg[QStringLiteral("parity")] = QStringLiteral("None");
    default_cfg[QStringLiteral("flow_control")] = QStringLiteral("None");

    // define the default values for output dump
    default_cfg[QStringLiteral("dump_enabled")] = QString::number(0);
    default_cfg[QStringLiteral("dump_file")] = QStringLiteral("cutecom-ng.dump");
    default_cfg[QStringLiteral("dump_format")] = QString::number(Raw);

    {
        //加载默认配置
        if(mainwindow!=NULL)
        {
            QDomDocument &doc=mainwindow->GetConfigDoc();
            QDomElement docroot=mainwindow->GetConfigRootNode();
            if(docroot.firstChildElement("connectdialog").isNull())
            {
                docroot.appendChild(doc.createElement("connectdialog"));
            }
            QDomElement root=docroot.firstChildElement("connectdialog");
            if(!root.isNull())
            {
                QDomElement defaultconfig=root.firstChildElement("default");
                if(!defaultconfig.isNull())
                {
                    QDomNamedNodeMap attrs=defaultconfig.attributes();
                    for(int i=0;i<attrs.size();i++)
                    {
                        QDomNode attr=attrs.item(i);
                        if(attr.nodeName()=="device")
                        {
                            //存在的设备名称设置才加载
                            QSerialPortInfo info(attr.nodeValue());
                            if(!info.isNull())
                            {
                               default_cfg[attr.nodeName()]=attr.nodeValue();
                            }
                            continue;
                        }
                        if(attr.nodeName()=="parity")
                        {
                            QString parity=attr.nodeValue();
                            bool isok=false;
                            int i=parity.toInt(&isok);
                            if(isok)
                            {
                               //符合QSerialPort::Parity定义
                               if(i==0)
                               {
                                   default_cfg[attr.nodeName()]=ui->parityList->itemText(0);
                               } else
                               {
                                   default_cfg[attr.nodeName()]=ui->parityList->itemText(i-1);
                               }
                            }
                            else
                            {
                               default_cfg[attr.nodeName()]=attr.nodeValue();
                            }
                            continue;
                        }
                        if(attr.nodeName()=="flow_control")
                        {
                            QString flow_control=attr.nodeValue();
                            bool isok=false;
                            int i=flow_control.toInt(&isok);
                            if(isok)
                            {
                               default_cfg[attr.nodeName()]=ui->flowControlList->itemText(i);
                            }
                            else
                            {
                               default_cfg[attr.nodeName()]=attr.nodeValue();
                            }
                            continue;
                        }
                        default_cfg[attr.nodeName()]=attr.nodeValue();
                    }
                }
            }
        }

    }


    preselectPortConfig(default_cfg);
}

void ConnectDialog::accept()
{
    // create a serial port config object from current selection
    QHash<QString, QString> cfg;
    cfg[QStringLiteral("device")] = ui->deviceList->currentText();
    cfg[QStringLiteral("baud_rate")] = ui->baudRateList->currentText();
    cfg[QStringLiteral("data_bits")] = ui->dataBitsList->currentText();
    cfg[QStringLiteral("stop_bits")] = ui->stopBitsList->itemData(
                ui->stopBitsList->currentIndex()).toString();
    cfg[QStringLiteral("parity")] = ui->parityList->itemData(
                ui->parityList->currentIndex()).toString();
    cfg[QStringLiteral("flow_control")] = ui->flowControlList->itemData(
                ui->flowControlList->currentIndex()).toString();
    cfg[QStringLiteral("dump_enabled")] = ui->dumpFile->isChecked() ? "1" : "0";
    cfg[QStringLiteral("dump_file")] = ui->dumpPath->text();
    cfg[QStringLiteral("dump_format")] = QString::number(ui->dumpRawFmt->isChecked() ? Raw : Ascii);

    hide();

    {
        //保存默认配置
        if(mainwindow!=NULL)
        {
            QDomDocument &doc=mainwindow->GetConfigDoc();
            QDomElement docroot=mainwindow->GetConfigRootNode();
            if(docroot.firstChildElement("connectdialog").isNull())
            {
                docroot.appendChild(doc.createElement("connectdialog"));
            }
            QDomElement root=docroot.firstChildElement("connectdialog");
            if(!root.isNull())
            {
                QDomElement defaultconfig=root.firstChildElement("default");
                if(defaultconfig.isNull())
                {
                    defaultconfig=doc.createElement("default");
                }
                for(auto it=cfg.begin();it!=cfg.end();it++)
                {
                    defaultconfig.setAttribute(it.key(),it.value());
                }
                root.appendChild(defaultconfig);
            }
        }
    }

    emit openDeviceClicked(cfg);
}

void ConnectDialog::paintEvent(QPaintEvent *event)
{
    //刷新翻译
    ui->retranslateUi(this);
    QDialog::paintEvent(event);
}
