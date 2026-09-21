#include "dlt645sessionmanager.h"
#include <QApplication>

dlt645sessionmanager::dlt645sessionmanager(QObject *parent)
    : QObject{parent},session_thread(new QThread(this)),session_loop_timer(new QTimer(this))
{
    memset(&dlt645,0,sizeof(dlt645));
    moveToThread(session_thread);
    connect(session_thread,&QThread::started,[=]()
    {
        connect(session_loop_timer,&QTimer::timeout,this,&dlt645sessionmanager::session_loop_timer_timeout);
        session_loop_timer->setSingleShot(false);
        session_loop_timer->start(5);
    });
    dlt645.io.send=[](struct hdlt645_master_io *io,uint8_t *frame,size_t frame_len) -> size_t
    {
        if(io->usr==NULL)
        {
            return 0;
        }
        dlt645sessionmanager & obj=*(dlt645sessionmanager *)io->usr;

        QByteArray data((const char *)frame,frame_len);

        obj.dataSend(data);

        return frame_len;
    };

    dlt645.io.timeout=[](struct hdlt645_master_io *io) -> size_t
    {
        if(io->usr==NULL)
        {
            return 0;
        }
        dlt645sessionmanager & obj=*(dlt645sessionmanager *)io->usr;

        if((std::chrono::steady_clock::now()-obj.dlt645_rx_tp) > std::chrono::milliseconds(500))
        {
            return 1;
        }

        return 0;
    };

    dlt645.io.usr=this;

    session_thread->start();
}

hdlt645_master_ctx_status_t dlt645sessionmanager::dlt645_status(void)
{
    return hdlt645_master_ctx_status(&dlt645.ctx);
}

void dlt645sessionmanager::session_loop_timer_timeout(void)
{
    hdlt645_master_ctx_process(&dlt645.ctx,&dlt645.io);
}

void dlt645sessionmanager::dataSend(const QByteArray data)
{
    emit sendToSerial(data);
}

void  dlt645sessionmanager::dataReceived(const QByteArray &data)
{
    if(!data.isEmpty())
    {
        dlt645_rx_tp=std::chrono::steady_clock::now();
        hdlt645_master_io_rx_input(&dlt645.io,(uint8_t *)data.data(),data.length());
    }
}



void dlt645sessionmanager::CloseSession()
{
    if(session_loop_timer!=NULL)
    {
        session_loop_timer->stop();
        delete session_loop_timer;
        session_loop_timer = NULL;
    }

    moveToThread(QApplication::instance()->thread());

    deleteLater();

    /*
     *退出当前线程
    */
    QThread::currentThread()->exit(0);

}

dlt645sessionmanager::~dlt645sessionmanager()
{

}