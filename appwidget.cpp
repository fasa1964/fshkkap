#include "appwidget.h"
#include "ui_appwidget.h"

AppWidget::AppWidget(const QString &NAME, const QString &VERSION, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppWidget)
{
    ui->setupUi(this);
    ui->nameLabel->setText(NAME);
    ui->versionLabel->setText("Version "+VERSION);
}

AppWidget::~AppWidget()
{
    delete ui;
}
