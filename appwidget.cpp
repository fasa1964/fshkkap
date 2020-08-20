#include "appwidget.h"
#include "ui_appwidget.h"

AppWidget::AppWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppWidget)
{
    ui->setupUi(this);
}

AppWidget::~AppWidget()
{
    delete ui;
}
