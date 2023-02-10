#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "Version.h"
#include "QFile"
#include "QDir"
#include "mainwindow.h"
#include "qscintillaedit.h"

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
        ui->license->styleClearAll();
        //tab为4空格
        ui->license->setTabWidth(4);
        //行高亮
        ui->license->setCaretLineVisible(true);
        //设置margin
        ui->license->setMarginWidthN(0,0);
        ui->license->setMarginWidthN(1,0);
        ui->license->setMarginWidthN(2,0);
        ui->license->setMarginWidthN(3,0);
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
    {
        //configdoc页
        ui->configdoc->styleClearAll();
        ui->configdoc->setILexer((sptr_t)(void *)CreateLexer("xml"));
        //设置property
        ui->configdoc->setProperty("fold","1");
        ui->configdoc->setProperty("fold.compact","0");
        ui->configdoc->setProperty("fold.xml.at.tag.open","1");
        ui->configdoc->setProperty("lexer.xml.allow.scripts","1");
        //设置foldflags
        ui->configdoc->setFoldFlags(SC_FOLDFLAG_LINEAFTER_CONTRACTED);

        //tab为4空格
        ui->configdoc->setTabWidth(4);
        //行高亮
        ui->configdoc->setCaretLineVisible(true);
        //设置margin
        ui->configdoc->setMarginTypeN(0,SC_MARGIN_NUMBER);
        ui->configdoc->setMarginWidthN(0,32);
        ui->configdoc->setMarginWidthN(1,0);
        ui->configdoc->setMarginTypeN(1,SC_MARGIN_SYMBOL);
        ui->configdoc->setMarginMaskN(1,SC_MASK_FOLDERS);
        ui->configdoc->setMarginWidthN(1,20);
        ui->configdoc->setMarginWidthN(2,0);
        ui->configdoc->setMarginWidthN(3,0);
        //定义marker
        ui->configdoc->markerDefine(SC_MARKNUM_FOLDER,SC_MARK_BOXPLUS);
        ui->configdoc->markerDefine(SC_MARKNUM_FOLDEROPEN,SC_MARK_BOXMINUS);
        ui->configdoc->markerDefine(SC_MARKNUM_FOLDEREND,SC_MARK_EMPTY);
        ui->configdoc->markerDefine(SC_MARKNUM_FOLDERMIDTAIL,SC_MARK_EMPTY);
        ui->configdoc->markerDefine(SC_MARKNUM_FOLDEROPENMID,SC_MARK_EMPTY);
        ui->configdoc->markerDefine(SC_MARKNUM_FOLDERSUB,SC_MARK_EMPTY);
        ui->configdoc->markerDefine(SC_MARKNUM_FOLDERTAIL,SC_MARK_EMPTY);
        //设置xml的style(XML对应的lexer为html)
        ui->configdoc->styleSetFore(SCE_H_DEFAULT,0);
        ui->configdoc->styleSetFore(SCE_H_TAG,0x000080);
        ui->configdoc->styleSetFore(SCE_H_TAGUNKNOWN,0x000080);
        ui->configdoc->styleSetFore(SCE_H_ATTRIBUTE,0x008080);
        ui->configdoc->styleSetFore(SCE_H_ATTRIBUTEUNKNOWN,0x008080);
        ui->configdoc->styleSetFore(SCE_H_NUMBER,0x800000);
        ui->configdoc->styleSetFore(SCE_H_DOUBLESTRING,0x800000);
        ui->configdoc->styleSetFore(SCE_H_SINGLESTRING,0x800000);
        ui->configdoc->styleSetFore(SCE_H_OTHER,0x800080);
        ui->configdoc->styleSetFore(SCE_H_COMMENT,0x000080);
        ui->configdoc->styleSetFore(SCE_H_ENTITY,0x80080);
        ui->configdoc->styleSetFore(SCE_H_TAGEND,0x00080);



        MainWindow *mainwindow=dynamic_cast<MainWindow *>(parent);
        if(mainwindow!=NULL)
        {
            QString configdoc=mainwindow->GetConfigDoc().toString();
            ui->configdoc->appendText(configdoc.length(),configdoc.toStdString().c_str());
            ui->configdoc->setReadOnly(true);
        }
    }
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
