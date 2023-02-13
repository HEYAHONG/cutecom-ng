#include "nulltransfer.h"
#include <QThread>

NullTransfer::NullTransfer(QObject *parent, QSerialPort *serial, const QString &filename)
    : FileTransfer(parent,serial,filename)
{

}

void NullTransfer::performTransfer()
{
    emit transferProgressed(100);
    emit transferEnded(NoError);
}

NullTransfer::~NullTransfer()
{

}
