#ifndef DLT645DIALOG_H
#define DLT645DIALOG_H

#include <QDialog>
#include <dlt645sessionmanager.h>

class MainWindow;

namespace Ui {
class dlt645dialog;
}

class dlt645dialog : public QDialog
{
    Q_OBJECT
    MainWindow *m_mainwindow;
    dlt645sessionmanager *session;
public:
    explicit dlt645dialog(MainWindow *parent = nullptr);
    ~dlt645dialog();

    void retranslateUi();

    /*
     *配置文件相关
     */
    void loadconfig();
    void saveconfig();

private:
    Ui::dlt645dialog *ui;
public slots:
    void sendToSerial(const QByteArray &data);
};

#endif // DLT645DIALOG_H
