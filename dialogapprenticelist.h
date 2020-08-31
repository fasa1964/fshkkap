#ifndef DIALOGAPPRENTICELIST_H
#define DIALOGAPPRENTICELIST_H

#include <QDialog>

namespace Ui {
class DialogApprenticeList;
}

class DialogApprenticeList : public QDialog
{
    Q_OBJECT

public:
    explicit DialogApprenticeList(QWidget *parent = 0);
    ~DialogApprenticeList();

private:
    Ui::DialogApprenticeList *ui;
};

#endif // DIALOGAPPRENTICELIST_H
