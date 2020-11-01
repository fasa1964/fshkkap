#include "dialogappsettings.h"
#include "ui_dialogappsettings.h"

DialogAppSettings::DialogAppSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAppSettings)
{
    ui->setupUi(this);
}

DialogAppSettings::~DialogAppSettings()
{
    delete ui;
}
