#include "modbusrtuwritedatadialog.h"
#include "ui_modbusrtuwritedatadialog.h"
#include <QStringListModel>
#include <QMessageBox>

ModbusRTUWriteDataDialog::ModbusRTUWriteDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModbusRTUWriteDataDialog)
{
    ui->setupUi(this);
    ui->listView->setModel((db_list=new QStringListModel(ui->listView)));
    ui->listView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
}

ModbusRTUWriteDataDialog::~ModbusRTUWriteDataDialog()
{
    delete ui;
}

int ModbusRTUWriteDataDialog::GetStartAddr()
{
    bool isok=false;
    int val=-1;
    if(ui->IsHex->isChecked())
    {
        val=ui->lineEdit->text().toInt(&isok,16);
    }
    else
    {
        val=ui->lineEdit->text().toInt(&isok,10);
    }
    return val;
}

void ModbusRTUWriteDataDialog::on_lineEdit_textChanged(const QString &arg1)
{
    bool isok=false;
    int val=-1;
    if(ui->IsHex->isChecked())
    {
        val=arg1.toInt(&isok,16);
    }
    else
    {
        val=arg1.toInt(&isok,10);
    }

    if(!isok || val < 0 || val > 0xFFFF)
    {
        ui->lineEdit->undo();
    }
}

QVector<uint16_t> ModbusRTUWriteDataDialog::GetData()
{
    return data;
}

void ModbusRTUWriteDataDialog::on_AddButton_clicked()
{
    if(db_list!=NULL)
    {
        QStringList list=db_list->stringList();
        if(ui->listView->currentIndex().isValid())
        {
           int row=ui->listView->currentIndex().row();
           if(row >= 0)
           {
               list.insert(row,"0");
           }
           else
           {
               list.append("0");
           }
        }
        else
        {
            list.append("0");
        }
        db_list->setStringList(list);
    }
}


void ModbusRTUWriteDataDialog::on_DelButton_clicked()
{
    if(db_list!=NULL)
    {
        QStringList list=db_list->stringList();
        if(ui->listView->currentIndex().isValid())
        {
           int row=ui->listView->currentIndex().row();
           if(row >= 0)
           {
               list.erase(list.begin()+row);
               db_list->setStringList(list);
           }
        }
    }
}


void ModbusRTUWriteDataDialog::on_OkButton_clicked()
{
    QStringList list=db_list->stringList();
    if(list.isEmpty())
    {
        QMessageBox::information(this,tr("information"),tr("Data is empty"));
        return;
    }

    data.clear();
    {
        for(auto it=list.begin();it!=list.end();it++)
        {
            bool isok=false;
            int val=-1;
            if(ui->IsHexData->isChecked())
            {
                val=(*it).toInt(&isok,16);
            }
            else
            {
                val=(*it).toInt(&isok,10);
            }
            if(isok && (0 <= val ) && (0xFFFF >= val))
            {
                data.push_back(val);
            }
            else
            {
                QMessageBox::information(this,tr("information"),tr("Data is invalid"));
                return;
            }
        }
    }

    accept();
}

