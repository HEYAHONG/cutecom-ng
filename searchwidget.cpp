#include "ui_searchwidget.h"
#include "searchwidget.h"

SearchWidget::SearchWidget(QFrame*parent) : QFrame(parent),ui(new Ui::SearchWidget)
{
    ui->setupUi((QFrame *)this);
}

SearchWidget::~SearchWidget()
{
    delete  ui;
}
