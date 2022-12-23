#include "ui_searchwidget.h"
#include "searchwidget.h"

SearchWidget::SearchWidget(QFrame*parent) : QFrame(parent),ui(new Ui::SearchWidget)
{
    ui->setupUi((QFrame *)this);
}

void SearchWidget::paintEvent(QPaintEvent *event)
{
    //刷新翻译
    ui->retranslateUi(this);
    QFrame::paintEvent(event);
}

SearchWidget::~SearchWidget()
{
    delete  ui;
}
