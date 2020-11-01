#include "dialogappsettings.h"
#include "ui_dialogappsettings.h"

#include <QIcon>
#include <QFileDialog>

DialogAppSettings::DialogAppSettings(const QMap<QString, QVariant> &map, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAppSettings)
{
    ui->setupUi(this);
    setWindowTitle(tr("Programm-Einstellungen"));
    setWindowIcon(QIcon(":/FSHKIcon.svg"));

    ui->orgNameEdit->setText( map.value("orgname").toString());
    ui->appPathEdit->setText( map.value("path").toString());

    connect(ui->cancelButton, &QPushButton::clicked, this, &DialogAppSettings::reject);
    connect(ui->applyButton, &QPushButton::clicked, this, &DialogAppSettings::accept);
    connect(ui->pathButton, &QToolButton::clicked, this,&DialogAppSettings::pathButtonClicked);

}

DialogAppSettings::~DialogAppSettings()
{
    delete ui;
}

QMap<QString, QVariant> DialogAppSettings::verfy()
{
    QMap<QString, QVariant> map;

    map.insert("orgname", ui->orgNameEdit->text());
    map.insert("path", ui->appPathEdit->text());

    return map;
}

void DialogAppSettings::pathButtonClicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Verzeichnis"));
    ui->appPathEdit->setText( path );
}
