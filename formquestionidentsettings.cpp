#include "formquestionidentsettings.h"
#include "ui_formquestionidentsettings.h"

FormQuestionIdentSettings::FormQuestionIdentSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormQuestionIdentSettings)
{
    ui->setupUi(this);
}

FormQuestionIdentSettings::~FormQuestionIdentSettings()
{
    delete ui;
}
