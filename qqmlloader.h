#ifndef QQMLLOADER_H
#define QQMLLOADER_H

#include <QQuickWidget>
#include <QDialog>
#include <QVariant>
#include <QMutex>
#include <QJSValue>
#include <QTimer>
#include <sessionmanager.h>

class QQmlLoader:public QDialog
{
    Q_OBJECT

    /*
     *QT版本
    */
    Q_PROPERTY(QString QtVersion READ getQtVersion CONSTANT)

public:
    QQmlLoader(QWidget *parent);
    virtual ~QQmlLoader();

    void LoadQmlSource(QUrl qml_path);

    QQuickWidget::Status GetLoadStatus();

    //获取插件Url
    QUrl GetPluginUrl();

    //获取插件名称属性
    QString GetPluginName();

    //Qt版本
    QString QtVersion;
    QString getQtVersion();

    /*
     *Qml定时器
    */
    Q_INVOKABLE void SetTimerCallback(QJSValue callback);
    Q_INVOKABLE void StartTimer(int interval);
    Q_INVOKABLE void StopTimer();

    /*
     *串口相关
    */
    Q_INVOKABLE bool IsSerialOpen();
    Q_INVOKABLE void SetSerialDataCallback(QJSValue callback);
    Q_INVOKABLE bool SendSerialData(QString data);

private:
    QQuickWidget *quick;
    QUrl loadurl;

    void SetupQmlContext(QQmlContext *root);

    QVariant GetRootItemProperty(QString name);

    /*
     *调用QML中函数需要加的锁
    */
    QMutex QmlCallLock;

    /*
     *Qml定时器
    */
    QTimer QmlTimer;
    QJSValue QmlTimerCallback;

    /*
     *串口相关
    */
    SessionManager *serialsession;
    QJSValue QmlSerialDataCallback;

public slots:

    void    statusChanged(QQuickWidget::Status status);

    /*
     *定时器超时
    */
    void    TimerTimeout();

    /*
     *接收到串口数据
    */
    void handleDataReceived(const QByteArray &data);
};

#endif // QQMLLOADER_H
