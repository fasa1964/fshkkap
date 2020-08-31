#include "dialogapprenticelist.h"
#include "ui_dialogapprenticelist.h"

DialogApprenticeList::DialogApprenticeList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogApprenticeList)
{
    ui->setupUi(this);
}

DialogApprenticeList::~DialogApprenticeList()
{
    delete ui;
}
