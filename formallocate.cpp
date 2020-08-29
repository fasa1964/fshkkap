#include "formallocate.h"
#include "ui_formallocate.h"

FormAllocate::FormAllocate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAllocate)
{
    ui->setupUi(this);
}

FormAllocate::~FormAllocate()
{
    delete ui;
}
