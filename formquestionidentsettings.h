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
    explicit FormQuestionIdentSettings(QWidget *parent = 0);
    ~FormQuestionIdentSettings();

private:
    Ui::FormQuestionIdentSettings *ui;
};

#endif // FORMQUESTIONIDENTSETTINGS_H
