#include "formapprenticeresultlist.h"
#include "ui_formapprenticeresultlist.h"

FormApprenticeResultList::FormApprenticeResultList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormApprenticeResultList)
{
    ui->setupUi(this);
}

FormApprenticeResultList::~FormApprenticeResultList()
{
    delete ui;
}
