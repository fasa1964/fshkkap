#include "formquestionidentsettings.h"
#include "ui_formquestionidentsettings.h"

FormQuestionIdentSettings::FormQuestionIdentSettings(const QMap<QString, double> &imap, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FormQuestionIdentSettings)
{
    ui->setupUi(this);
    updateTable(imap);

    connect(ui->cancelButton, &QPushButton::clicked, this, &FormQuestionIdentSettings::reject);
    connect(ui->okButton, &QPushButton::clicked, this, &FormQuestionIdentSettings::accept);
}

FormQuestionIdentSettings::~FormQuestionIdentSettings()
{
    delete ui;
}

QMap<QString, double> FormQuestionIdentSettings::identMap()
{
    QMap<QString, double> iMap;

    for(int i = 0; i < ui->tableWidget->rowCount(); i++){
        double f = ui->tableWidget->item(i,1)->text().toDouble();
        QString key = ui->tableWidget->item(i,0)->text();
        iMap.insert( key, f);
    }

    return iMap;
}

void FormQuestionIdentSettings::updateTable(const QMap<QString, double> &map)
{
    ui->tableWidget->setRowCount( map.size() );
    ui->tableWidget->setColumnCount(2);

    QStringList headers;
    headers << "Kennung" << "Faktor";
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    int row = 0;
    QMapIterator<QString, double> it(map);
    while (it.hasNext()) {
        it.next();

        QTableWidgetItem *itemName = new QTableWidgetItem(it.key());
        QTableWidgetItem *itemFactor = new QTableWidgetItem( QString::number( it.value(), 'g', 3) );

        ui->tableWidget->setItem(row,0,itemName);
        ui->tableWidget->setItem(row,1,itemFactor);

        itemName->setFlags(Qt::ItemIsEnabled);
        itemFactor->setTextColor(Qt::blue);

        row++;
    }

    ui->tableWidget->resizeColumnsToContents();


}
