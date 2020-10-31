#ifndef DIALOGINFO_H
#define DIALOGINFO_H

#include <QDialog>

namespace Ui {
class DialogInfo;
}

class DialogInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DialogInfo(const QString &appname, const QString &appversion, QWidget *parent = 0);
    ~DialogInfo();

private slots:
    void qtButtonClicked();
    void updateButtonClicked();


private:
    Ui::DialogInfo *ui;
};

#endif // DIALOGINFO_H
