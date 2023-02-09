#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "Version.h"
#include "QFile"
#include "QDir"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    {
        //ABOUT页
        ui->version->setText(PROJECT_VERSION_STR);
    }
    {
        //LICENSE页
        {
            QFile file(":/LICENSE");
            file.open(QFile::ReadOnly);
            if(file.isOpen())
            {
                QString license=file.readAll();
                ui->license->addText(license.length(),license.toStdString().c_str());
            }
        }
        ui->license->setReadOnly(true);
    }
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
