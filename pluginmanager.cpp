#include "pluginmanager.h"
#include "ui_pluginmanager.h"
#include "mainwindow.h"
#include "qqmlloader.h"
#include "QFileDialog"
#include "QUrl"

PluginManager::PluginManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PluginManager),
    mainwindow(NULL)
{
    ui->setupUi(this);
    {
        //设置表格
        model=new QStandardItemModel(this);
        ui->tableView->setModel(model);
        connect(ui->tableView,&QTableView::clicked,this,&PluginManager::tableclicked);
    }
    setWindowTitle(tr("pluginmanager"));
    {
        MainWindow *ptr=dynamic_cast<MainWindow *>(parent);
        if(ptr!=NULL)
        {
            mainwindow=ptr;
            UpdateQmlList();
        }
    }
}

void PluginManager::UpdateQmlList()
{
    if(model!=NULL && mainwindow!=NULL)
    {
        model->clear();
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableView->verticalHeader()->hide();
        {
            model->setColumnCount(3);
            QStringList list;
            list.append(tr("name"));
            list.append(tr("type"));
            list.append(tr("path"));
            model->setHorizontalHeaderLabels(list);
        }
        ui->tableView->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
        for(auto it=mainwindow->qml_list.begin();it!=mainwindow->qml_list.end();it++)
        {
            QQmlLoader *loader=it->data();
            QList<QStandardItem *> row;
            row.append(new QStandardItem(loader->GetPluginName()));
            row.append(new QStandardItem("Qml"));
            row.append(new QStandardItem(loader->GetPluginUrl().toString()));
            model->appendRow(row);
        }
    }
}

PluginManager::~PluginManager()
{
    delete ui;
}

void PluginManager::on_pushButton_LoadQml_clicked()
{
    QUrl qml_path=QFileDialog::getOpenFileUrl(this,tr("qml path"));
    if(!qml_path.isEmpty())
    {
        if(mainwindow!=NULL)
            mainwindow->LoadQmlPlugin(qml_path,false);
    }
    UpdateQmlList();
}


void PluginManager::on_pushButton_UnloadQml_clicked()
{
    if(selectindex.isValid())
    {
        //获取url所在单元格
        auto qurlindex=model->index(selectindex.row(),2);
        if(qurlindex.isValid())
        {
            QString qml_path=qurlindex.data().toString();
            if(mainwindow!=NULL)
            {
                mainwindow->UnloadQmlPlugin(qml_path);
            }
        }
    }

    //清除已选单元格
    selectindex=QModelIndex();

    UpdateQmlList();
}

void PluginManager::tableclicked(const QModelIndex &index)
{
    if(index.isValid())
        selectindex=index;
}
