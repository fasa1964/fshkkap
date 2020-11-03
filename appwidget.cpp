#include "appwidget.h"
#include "ui_appwidget.h"

#include <QPainter>
#include <QPixmap>
#include <QImage>

AppWidget::AppWidget(const QString &NAME, const QString &VERSION, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppWidget)
{
    ui->setupUi(this);
    ui->nameLabel->setText(NAME);
    ui->versionLabel->setText("Version "+VERSION);

    ui->iconLabel->setMaximumWidth(250);
    ui->iconLabel->setMaximumHeight(250);

}

AppWidget::~AppWidget()
{
    delete ui;
}

void AppWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, QPixmap(":/images/BackgroundApp.svg").scaled(size()));
    ui->iconLabel->setMaximumWidth(ui->nameLabel->width());
    ui->iconLabel->setMaximumHeight(ui->nameLabel->width());
    QWidget::paintEvent(event);
}
