#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QObject>
#include <QFrame>
namespace Ui {
class SearchWidget;
}


class SearchWidget : public QFrame
{
    Q_OBJECT
public:
    explicit SearchWidget(QFrame *parent = nullptr);
    virtual ~SearchWidget();
private:
    Ui::SearchWidget *ui;
signals:

protected:
     void paintEvent(QPaintEvent *event);

};

#endif // SEARCHWIDGET_H
