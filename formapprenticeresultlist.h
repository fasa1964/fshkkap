#ifndef FORMAPPRENTICERESULTLIST_H
#define FORMAPPRENTICERESULTLIST_H

#include <QWidget>

namespace Ui {
class FormApprenticeResultList;
}

class FormApprenticeResultList : public QWidget
{
    Q_OBJECT

public:
    explicit FormApprenticeResultList(QWidget *parent = 0);
    ~FormApprenticeResultList();

private:
    Ui::FormApprenticeResultList *ui;
};

#endif // FORMAPPRENTICERESULTLIST_H
