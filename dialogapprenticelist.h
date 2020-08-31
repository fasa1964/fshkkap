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
    explicit DialogApprenticeList(const QString &title, const QString &message, const QStringList &names, QWidget *parent = 0);
    ~DialogApprenticeList();

private:
    Ui::DialogApprenticeList *ui;
    void setupListWidget(const QStringList &list);
};

#endif // DIALOGAPPRENTICELIST_H
