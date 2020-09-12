#ifndef FORMQUESTIONIDENTSETTINGS_H
#define FORMQUESTIONIDENTSETTINGS_H

#include <QDialog>

namespace Ui {
class FormQuestionIdentSettings;
}

class FormQuestionIdentSettings : public QDialog
{
    Q_OBJECT

public:
    explicit FormQuestionIdentSettings(const QMap<QString, double> &imap, QWidget *parent = 0);
    ~FormQuestionIdentSettings();

    QMap<QString, double> identMap();

private:
    Ui::FormQuestionIdentSettings *ui;

    void updateTable(const QMap<QString, double> &map);
};

#endif // FORMQUESTIONIDENTSETTINGS_H
