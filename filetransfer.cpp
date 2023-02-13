/**
 * \file
 * <!--
 * Copyright 2015 Develer S.r.l. (http://www.develer.com/)
 * -->
 *
 * \brief FileTransfer class implementation
 *
 * \author Aurelien Rainone <aurelien@develer.com>
 */

#include "filetransfer.h"
#include <QApplication>
#include <QtSerialPort>
#include <QFile>

FileTransfer::FileTransfer(QObject *parent, QSerialPort *serial, const QString &filename) :
    QObject(parent),
    filename(filename),
    serial(serial),
    thread(0)
{
    total_size = 0;
    qRegisterMetaType<TransferError>("TransferError");
}

bool FileTransfer::startTransfer()
{   
    // fill buffer with file content
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly))
    {
        buffer = file.readAll();
        total_size = file.size();
        file.close();
        if (total_size > 0)
        {
            thread = new QThread;

            // and move both serialport and filetransfer (this) instance
            moveToThread(thread);
            serial->moveToThread(thread);

            // call child class performTransfer() when thread starts
            connect(thread, &QThread::started, this, &FileTransfer::performTransfer);

            connect(this, &FileTransfer::transferEnded,
                    this, &FileTransfer::handleTransferEnded);

            connect(thread, &QThread::finished, thread, &QThread::deleteLater);

            thread->start();
            return true;
        }
    }

    handleTransferEnded(InputFileError);
    return false;
}

void FileTransfer::handleTransferEnded(TransferError error)
{
    Q_UNUSED(error)

    // move serial and this instance back to main thread so...
    serial->moveToThread(QApplication::instance()->thread());
    moveToThread(QApplication::instance()->thread());

    // ... we can end the thread
    thread->quit();
}

QString FileTransfer::errorString(TransferError error)
{
    switch(error)
    {
        case NoError :
            return QString(tr("Success"));
        case NoSyncError:
            return QString(tr("Synchronization error"));
        case TransmissionError:
            return QString(tr("Transmission error"));
        case TimeoutError:
            return QString(tr("Transfer timeout"));
        case RemoteCancelledError:
            return QString(tr("Transfer cancelled by remote"));
        case LocalCancelledError:
            // should not be treated as an error
            return QString(tr("Transfer cancelled"));
        case InputFileError:
            return QString(tr("Can't open input file"));
        case UnknownError:
        default:
            return QString(tr("Unknown Error"));
    }
}
