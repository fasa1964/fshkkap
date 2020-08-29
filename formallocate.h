#ifndef FORMALLOCATE_H
#define FORMALLOCATE_H

#include <QWidget>

namespace Ui {
class FormAllocate;
}

class FormAllocate : public QWidget
{
    Q_OBJECT

public:
    explicit FormAllocate(QWidget *parent = 0);
    ~FormAllocate();

private:
    Ui::FormAllocate *ui;
};

#endif // FORMALLOCATE_H
