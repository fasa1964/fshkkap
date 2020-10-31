#include "dialoginfo.h"
#include "ui_dialoginfo.h"

#include <QMessageBox>

DialogInfo::DialogInfo(const QString &appname, const QString &appversion, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInfo)
{
    ui->setupUi(this);
    setWindowTitle(tr("Über FSHK-AP"));

    ui->labelName->setText(appname);
    ui->labelVersion->setText(appversion);


    connect(ui->closeButton, &QPushButton::clicked, this, &DialogInfo::close);
    connect(ui->updateButton, &QPushButton::clicked, this, &DialogInfo::updateButtonClicked);
    connect(ui->aboutQtButton, &QPushButton::clicked, this, &DialogInfo::qtButtonClicked);
}

DialogInfo::~DialogInfo()
{
    delete ui;
}

void DialogInfo::qtButtonClicked()
{
    QMessageBox::aboutQt(this);
}
void DialogInfo::updateButtonClicked()
{

}
