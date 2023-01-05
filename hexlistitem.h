#ifndef HEXLISTITEM_H
#define HEXLISTITEM_H

#include <QWidget>

namespace Ui {
class HexListItem;
}

class HexListItem : public QWidget
{
    Q_OBJECT

public:
    explicit HexListItem(QWidget *parent = nullptr);
    virtual ~HexListItem();

    void SetData(const QByteArray &data);

private:
    Ui::HexListItem *ui;
};

#endif // HEXLISTITEM_H
