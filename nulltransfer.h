#ifndef NULLTRANSFER_H
#define NULLTRANSFER_H

#include "filetransfer.h"
#include <QObject>

class NullTransfer : public FileTransfer
{
    Q_OBJECT
public:
    explicit NullTransfer(QObject *parent, QSerialPort *serial, const QString &filename);
    ~NullTransfer();
private:
    void performTransfer();
};

#endif // NULLTRANSFER_H
