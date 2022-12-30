#ifndef QQMLLOADER_H
#define QQMLLOADER_H

#include <QQuickWidget>
#include <QDialog>


class QQmlLoader:public QDialog
{
    Q_OBJECT
public:
    QQmlLoader(QWidget *parent);
    virtual ~QQmlLoader();

    void LoadQmlSource(QUrl qml_path);

    QQuickWidget::Status GetLoadStatus();

private:
    QQuickWidget *quick;

    void SetupQmlContext(QQmlContext *root);
public slots:

    void 	statusChanged(QQuickWidget::Status status);
};

#endif // QQMLLOADER_H
