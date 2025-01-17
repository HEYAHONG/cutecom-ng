/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief SessionManager class implementation
 *
 * \author Aurelien Rainone <aurelien@develer.com>
 */

#include "sessionmanager.h"
#include "outputmanager.h"
#include "xmodemtransfer.h"
#include "nulltransfer.h"

#include <QCoreApplication>
#include <QSerialPortInfo>
#include <QProgressDialog>
#include <QMessageBox>
#include <QFile>

SessionManager::SessionManager(QObject *parent) :
    QObject(parent)
{
    serial = new QSerialPort();
    in_progress = false;
    file_transfer = 0;

    connect(serial, &QSerialPort::readyRead, this, &SessionManager::readData);
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>
                (&QSerialPort::errorOccurred), this, &SessionManager::handleError);
#else
    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>
                (&QSerialPort::error), this, &SessionManager::handleError);
#endif
}

SessionManager::~SessionManager()
{
    if (serial)
    {
        // closes connection if needed
        if (serial->isOpen())
            serial->close();
        delete serial;
    }
}

void SessionManager::handleError(QSerialPort::SerialPortError serialPortError)
{
    switch (serialPortError)
    {
        // no error
        case QSerialPort::NoError:
            break;

        // recoverable errors : inform user and clear error
        case QSerialPort::OpenError:

            QMessageBox::warning(NULL, tr("Error"), serial->errorString());
            // reset error
            serial->clearError();
            break;

        // unrecoverable errors : inform user and close the port/connection
        default:
            if (in_progress)
            {
                QMessageBox::critical(NULL, tr("Error"), serial->errorString());

                // on some error (ex: hot unplugging) the 'QSerialPort::error' property successively
                // takes multiple values.
                // to prevent from displaying successive error messages, the in_progress flag is
                // set to indicate that we are not interested by next messages, until the user tries to open
                // again the serial port
                in_progress = false;
                if (serial->isOpen())
                {
                    serial->clearError();

                    closeSession();
                }
            }
            break;
    }
}

void SessionManager::openSession(const QHash<QString, QString>& port_cfg)
{
    bool cfg_ok = true, ok;

    // try converting port config from the hash
    qint32 baud_rate = static_cast<qint32>
            (port_cfg["baud_rate"].toInt(&ok));
    cfg_ok &= ok;

    QSerialPort::DataBits data_bits = static_cast<QSerialPort::DataBits>
            (port_cfg["data_bits"].toInt(&ok));
    cfg_ok &= ok;

    QSerialPort::Parity parity = static_cast<QSerialPort::Parity>
            (port_cfg["parity"].toInt(&ok));
    cfg_ok &= ok;

    QSerialPort::StopBits stop_bits = static_cast<QSerialPort::StopBits>
            (port_cfg["stop_bits"].toInt(&ok));
    cfg_ok &= ok;

    QSerialPort::FlowControl flow_control = static_cast<QSerialPort::FlowControl>
            (port_cfg["flow_control"].toInt(&ok));
    cfg_ok &= ok;

    // a conversion didn't make it
    Q_ASSERT_X(cfg_ok, "SessionManager::openSession", "a conversion didn't make it");

    // configure port
#if (QT_VERSION < QT_VERSION_CHECK(5, 5, 0)) && defined(Q_OS_MAC)
    // 'device not found' error on MacOsX when we try to open port
    // after calling setPortName with a full device name (eg: /dev/ttyUSB01)

    // issues:
    // - https://github.com/develersrl/cutecom-ng/issues/7
    // - https://github.com/develersrl/cutecom-ng/issues/17
    // this is a QSerialPort bug, corrected for Qt > 5.5:
    // - https://codereview.qt-project.org/#/c/108571/
    if (port_cfg[QStringLiteral("device")].contains('/'))
        serial->setPortName(port_cfg[QStringLiteral("device")]);
    else
        serial->setPort(QSerialPortInfo(port_cfg[QStringLiteral("device")]));
#else
    // tested on linux and windows
    // and this is necessary to make QSerialPort work with pseudo
    // terminal created with socat for example
    serial->setPortName(port_cfg[QStringLiteral("device")]);
#endif
    serial->setBaudRate(baud_rate);
    serial->setDataBits(data_bits);
    serial->setParity(parity);
    serial->setStopBits(stop_bits);
    serial->setFlowControl(flow_control);
    // flag indicating that a connection is in progress (eventually successful or not)
    in_progress = true;

    // open serial port
    if (serial->open(QIODevice::ReadWrite))
    {
        curr_cfg = port_cfg;
        bytesRead=0;
        bytesWrite=0;
        emit statisticChanged(bytesRead,bytesWrite);
        emit sessionOpened();
    }
    else
    {
        // here, stopped means 'no connection is in progress'
        emit sessionClosed();
    }
}

void SessionManager::closeSession()
{
    if (serial->isOpen())
    {
        serial->close();
        emit sessionClosed();
    }
}

bool SessionManager::isSessionOpen() const
{
    return serial->isOpen();
}

void SessionManager::saveToFile(const QByteArray &data)
{
    QFile dump(curr_cfg["dump_file"]);

    // mode is OR'ed with 'Text' flag in "Ascii" mode
    QIODevice::OpenMode mode = QIODevice::Append;

    if (curr_cfg["dump_file"] == QString::number(ConnectDialog::Ascii))
        mode |= QIODevice::Text;

    if (dump.open(mode))
        dump.write(data);
}

void SessionManager::readData()
{
    QByteArray data(serial->readAll());

    addbytesRead(data.size());

    emit dataReceived(data);

    // append to dump file if configured
    if (curr_cfg["dump_enabled"] == "1")
        saveToFile(data);
}

void SessionManager::sendToSerial(const QByteArray &data)
{
    addbytesWrite(data.size());

    serial->write(data);
}

void SessionManager::transferFile(const QString &filename, Protocol type)
{
    switch (type)
    {
        case XMODEM:
            file_transfer = new XModemTransfer(0, serial, filename);
        break;
        case YMODEM:
        case ZMODEM:
        default:
            QMessageBox::warning(0,QString(tr("warning")),QString(tr("not implemented")));
            file_transfer = new NullTransfer(0,serial,filename);
    }

    connect(file_transfer, &FileTransfer::transferEnded,
            this, &SessionManager::handleFileTransferEnded);

    // forward FileTransfer::transferProgressed signals
    connect(file_transfer, &FileTransfer::transferProgressed,
            this, &SessionManager::fileTransferProgressed);
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    disconnect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>
                (&QSerialPort::errorOccurred), this, &SessionManager::handleError);
#else
    disconnect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>
                (&QSerialPort::error), this, &SessionManager::handleError);
#endif
    // perform transfer
    if (!file_transfer->startTransfer())
        // transfer never started, manually delete FileTransfer instance
        delete file_transfer;
}

void SessionManager::handleFileTransferEnded(FileTransfer::TransferError error)
{
    // re-connect serial port error handling for non-blocking use
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>
                (&QSerialPort::errorOccurred), this, &SessionManager::handleError);
#else
    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>
                (&QSerialPort::error), this, &SessionManager::handleError);
#endif
    // schedule file_transfer object deletion on main thread
    QCoreApplication::postEvent(file_transfer, new QEvent(QEvent::DeferredDelete));
    emit fileTransferEnded(error);
}


void SessionManager::handleTransferCancelledByUser()
{
    Q_ASSERT_X(file_transfer, "SessionManager::handleTransferCancelledByUser",
               "file_transfer shound not be NULL");
    file_transfer->quit_requested = true;
}


void SessionManager::setRTS(bool RTS)
{
    if(serial!=NULL)
    {
        serial->setRequestToSend(RTS);
    }
}

bool SessionManager::getRTS()
{
    if(serial!=NULL && serial->isOpen())
    {
        return serial->isRequestToSend();
    }
    return false;
}


void SessionManager::setDTR(bool DTR)
{
    if(serial!=NULL)
    {
        serial->setDataTerminalReady(DTR);
    }
}

bool SessionManager::getDTR()
{
    if(serial!=NULL && serial->isOpen())
    {
        return serial->isDataTerminalReady();
    }
    return false;
}

QSerialPortInfo SessionManager::getInfo()
{
    if(serial==NULL || !serial->isOpen())
    {
        return QSerialPortInfo();
    }
    return QSerialPortInfo(*serial);
}

QSerialPort & SessionManager::getSerialPort()
{
    return *serial;
}

void SessionManager::addbytesRead(int size)
{
    bytesRead+=size;

    emit statisticChanged(bytesRead,bytesWrite);
}

void SessionManager::addbytesWrite(int size)
{
    bytesWrite+=size;

    emit statisticChanged(bytesRead,bytesWrite);
}
