#include "dialogapprenticelist.h"
#include "ui_dialogapprenticelist.h"

DialogApprenticeList::DialogApprenticeList(const QString &title, const QString &message, const QStringList &names, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogApprenticeList)
{
    ui->setupUi(this);

    ui->titleLabel->setText(title);
    ui->messageLabel->setText(message);
    setupListWidget(names);

    connect(ui->cancelButton, &QPushButton::clicked, this, &DialogApprenticeList::reject);
    connect(ui->okButton, &QPushButton::clicked, this, &DialogApprenticeList::accept);

}

DialogApprenticeList::~DialogApprenticeList()
{
    delete ui;
}

void DialogApprenticeList::setupListWidget(const QStringList &list)
{
    ui->listWidget->clear();
    for(int i = 0; i < list.size(); i++)
        ui->listWidget->addItem(new QListWidgetItem(list.at(i)));
}
