#ifndef QQMLLOADER_H
#define QQMLLOADER_H

#include <QQuickWidget>
#include <QDialog>
#include <QVariant>


class QQmlLoader:public QDialog
{
    Q_OBJECT
public:
    QQmlLoader(QWidget *parent);
    virtual ~QQmlLoader();

    void LoadQmlSource(QUrl qml_path);

    QQuickWidget::Status GetLoadStatus();

    //获取插件名称属性
    QString GetPluginName();

private:
    QQuickWidget *quick;

    void SetupQmlContext(QQmlContext *root);

    QVariant GetRootItemProperty(QString name);

public slots:

    void 	statusChanged(QQuickWidget::Status status);
};

#endif // QQMLLOADER_H
