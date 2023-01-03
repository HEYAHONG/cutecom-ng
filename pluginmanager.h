#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class PluginManager;
}

class MainWindow;

class PluginManager : public QDialog
{
    Q_OBJECT

public:
    explicit PluginManager(QWidget *parent = nullptr);
    ~PluginManager();

private slots:
    void on_pushButton_LoadQml_clicked();

    void on_pushButton_UnloadQml_clicked();

private:
    Ui::PluginManager *ui;

    MainWindow *mainwindow;
    QStandardItemModel *model;

    void UpdateQmlList();
};

#endif // PLUGINMANAGER_H
