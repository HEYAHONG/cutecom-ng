#ifndef DLT645SESSIONMANAGER_H
#define DLT645SESSIONMANAGER_H

#include <QObject>
#include <QQmlEngine>
#include <QThread>
#include <QTimer>
#include "HCPPBox.h"
#include <chrono>

class dlt645sessionmanager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QThread *session_thread;
    QTimer  *session_loop_timer;
    struct
    {
        hdlt645_master_io_t  io;
        hdlt645_master_ctx_t ctx;
    } dlt645;
    std::chrono::time_point<std::chrono::steady_clock> dlt645_rx_tp;
    /*
     * 数据发送
     */
    void dataSend(const QByteArray data);
public:
    explicit dlt645sessionmanager(QObject *parent = nullptr);
    virtual ~dlt645sessionmanager();

    hdlt645_master_ctx_status_t dlt645_status(void);

signals:

    /*
     * 发送到串口
     */
    void sendToSerial(const QByteArray &data);

private slots:
    void session_loop_timer_timeout();
public slots:


    /*
     * 数据接收
     */
    void dataReceived(const QByteArray &data);


    /*
     *关闭会话（不可直接使用delete删除）
     */
    void CloseSession();
};

#endif // DLT645SESSIONMANAGER_H
