#include "formcompany.h"
#include "ui_formcompany.h"

#include <QMessageBox>
#include <QDebug>

FormBetrieb::FormBetrieb(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormBetrieb)
{
    ui->setupUi(this);

    setFormReadOnly(true);
    selectedCompany = ClassBetrieb();

    QStringList labels;
    labels << "Alle" << "Betriebe mit Auszubildenden";
    ui->sortBox->addItems(labels);

    connect(ui->closeButton, &QPushButton::clicked, this, &FormBetrieb::closeButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &FormBetrieb::deleteButtonClicked);
    connect(ui->createButton, &QPushButton::clicked, this, &FormBetrieb::createButtonClicked);
    connect(ui->changeButton, &QPushButton::clicked, this, &FormBetrieb::changeButtonClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &FormBetrieb::saveButtonClicked);

    connect(ui->sortBox, &QComboBox::currentTextChanged, this, &FormBetrieb::sortBoxTextChanged);
    connect(ui->companyTableWidget, &QTableWidget::itemClicked, this, &FormBetrieb::companyTableItemClicked);

}

FormBetrieb::~FormBetrieb()
{
    delete ui;
}

void FormBetrieb::closeButtonClicked()
{

    close();
    emit companyFormClosed();
}

void FormBetrieb::createButtonClicked()
{
    clearForm();
    setFormReadOnly(false);
    ui->nameEdit->setFocus();

    selectedCompany = ClassBetrieb();
    ui->nrBox->setValue(getNextKey());

    ui->saveButton->setEnabled(true);
    ui->createButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    ui->changeButton->setEnabled(false);
    ui->deleteApprenticeButton->setEnabled(false);
}

void FormBetrieb::deleteButtonClicked()
{
    if(selectedCompany.name().isEmpty())
        return;

    int result = QMessageBox::information(this, tr("Löschen Betrieb"), tr("Der Betrieb: ")+ selectedCompany.name()
                                          + tr("\nwird unwiderruflich gelöscht!"),
                                       QMessageBox::Yes | QMessageBox::Abort);
    if(result == QMessageBox::Abort)
        return;

    if(m_companyMap.remove(selectedCompany.nr()) == 0){
        QMessageBox::information(this, tr("Löschen Betrieb"), tr("Fehler beim Löschen. Bitte starten sie das Programm neu!"));
        return;
    }

    if(selectedCompany.azubiMap().size() > 0)
        emit apprenticeRemoved(selectedCompany.azubiMap().values(), selectedCompany);


    updateCompanyTable(m_companyMap);
    clearForm();
    if(!m_companyMap.isEmpty()){
        selectedCompany = m_companyMap.values().first();
        setCompanyToForm(selectedCompany);
    }

    emit saveCompanyMap(m_companyMap);

}

void FormBetrieb::changeButtonClicked()
{

    if(selectedCompany.name().isEmpty())
        return;

    setFormReadOnly(false);
    ui->nameEdit->setFocus();

    ui->saveButton->setEnabled(true);
    ui->createButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    ui->changeButton->setEnabled(false);
    ui->deleteApprenticeButton->setEnabled(false);
}

void FormBetrieb::saveButtonClicked()
{
    ClassBetrieb company = readFromForm();

    if(company.name().isEmpty() || company.city().isEmpty() || company.street().isEmpty()){
        QMessageBox::information(this, tr("Betrieb speichern"), tr("Der Name, Straße und Ort vom "
                                         "Betrieb müssen angegeben werden!"));
        return;
    }

    m_companyMap.insert(company.nr(), company);
    updateCompanyTable(m_companyMap);
    emit saveCompanyMap(m_companyMap);

    selectedCompany = company;

    setFormReadOnly(true);
    setCompanyToForm(selectedCompany);

    ui->saveButton->setEnabled(false);
    ui->createButton->setEnabled(true);
    ui->deleteButton->setEnabled(true);
    ui->changeButton->setEnabled(true);
    ui->deleteApprenticeButton->setEnabled(false);

}

void FormBetrieb::sortBoxTextChanged(const QString &text)
{
    if(companyMap().isEmpty() || text.isEmpty())
        return;

}

void FormBetrieb::companyTableItemClicked(QTableWidgetItem *item)
{
    clearForm();
    int key = ui->companyTableWidget->item(item->row(),0)->text().toInt();
    selectedCompany = m_companyMap.value(key);
    setCompanyToForm(selectedCompany);
    setFormReadOnly(true);
}

/// !brief When delete a company from list.
/// A consecutive numbering will be missing.
/// This function will discover the missing nr.
int FormBetrieb::getNextKey()
{
    int nr = m_companyMap.size() + 1;
    int row = 0;
    for(int i = 1; i < m_companyMap.size(); i++){
        if(i != ui->companyTableWidget->item(row, 0)->text().toInt() ){
            nr = i;
            break;
        }
        row++;
    }

    return nr;
}

void FormBetrieb::updateCompanyTable(const QMap<int, ClassBetrieb> bMap)
{
    ui->companyTableWidget->clear();
    ui->companyTableWidget->setColumnCount(2);
    ui->companyTableWidget->setRowCount(bMap.size());

    QStringList headers;
    headers << "Nr" << "Name";
    ui->companyTableWidget->setHorizontalHeaderLabels(headers);

    int row = 0;
    QMapIterator<int, ClassBetrieb> it(bMap);
    while (it.hasNext()) {
       it.next();
       QString nrs = QString::number(it.key(),10);

       QTableWidgetItem *itemNr = new QTableWidgetItem(nrs);
       QTableWidgetItem *itemName = new QTableWidgetItem(it.value().name());

       ui->companyTableWidget->setItem(row,0, itemNr);
       ui->companyTableWidget->setItem(row,1, itemName);

       itemName->setFlags(Qt::ItemIsEnabled);
       itemNr->setFlags(Qt::ItemIsEnabled);

       row++;
    }

    ui->companyTableWidget->resizeColumnToContents(0);
    ui->companyTableWidget->resizeColumnToContents(1);
}

void FormBetrieb::updateApprenticeTable(const QMap<QString, ClassLehrling> aMap)
{
    ui->apprenticeTableWidget->clear();
    ui->apprenticeTableWidget->setColumnCount(2);
    ui->apprenticeTableWidget->setRowCount(aMap.size());

    QStringList headers;
    headers << "Kennung" << "Klasse";
    ui->apprenticeTableWidget->setHorizontalHeaderLabels(headers);

    int row = 0;
    QMapIterator<QString, ClassLehrling> it(aMap);
    while (it.hasNext()) {
       it.next();

       QTableWidgetItem *itemKennung = new QTableWidgetItem(it.value().getKey());
       QTableWidgetItem *itemKlasse = new QTableWidgetItem(it.value().educationClass());

       ui->apprenticeTableWidget->setItem(row,0, itemKennung);
       ui->apprenticeTableWidget->setItem(row,1, itemKlasse);

       row++;
    }

    ui->apprenticeTableWidget->resizeColumnToContents(0);
    ui->apprenticeTableWidget->resizeColumnToContents(1);
}

QMap<int, ClassBetrieb> FormBetrieb::companyMap() const
{
    return m_companyMap;
}

void FormBetrieb::setCompanyMap(const QMap<int, ClassBetrieb> &companyMap)
{
    m_companyMap = companyMap;
    if(!m_companyMap.isEmpty()){
        updateCompanyTable(m_companyMap);
        selectedCompany = m_companyMap.values().first();
        setCompanyToForm(selectedCompany);
    }

}

ClassBetrieb FormBetrieb::readFromForm()
{
    ClassBetrieb company;
    company.setNr(ui->nrBox->value());
    company.setName(ui->nameEdit->text());
    company.setCity(ui->ortEdit->text());
    company.setStreet(ui->strasseEdit->text());
    company.setPhone(ui->phoneEdit->text());

    return company;
}

void FormBetrieb::setCompanyToForm(const ClassBetrieb &company)
{
    ui->nrBox->setValue(company.nr());
    ui->nameEdit->setText(company.name());
    ui->strasseEdit->setText(company.street());
    ui->ortEdit->setText(company.city());
    ui->phoneEdit->setText(company.phone());

    if(!company.azubiMap().isEmpty())
       updateApprenticeTable(company.azubiMap());


    ui->saveButton->setEnabled(false);
    ui->createButton->setEnabled(true);
    ui->deleteButton->setEnabled(true);
    ui->changeButton->setEnabled(true);
    ui->deleteApprenticeButton->setEnabled(false);
}

void FormBetrieb::setFormReadOnly(bool status)
{
    ui->nameEdit->setReadOnly(status);
    ui->strasseEdit->setReadOnly(status);
    ui->ortEdit->setReadOnly(status);
    ui->phoneEdit->setReadOnly(status);

    if(!status)
        setFormTextColor(QColor(0,85,127));
    else
        setFormTextColor(Qt::black);

}

void FormBetrieb::setFormTextColor(QColor color)
{
    QPalette palette;
    palette.setColor(QPalette::Base,Qt::white);
    palette.setColor(QPalette::Text,color);

    ui->nrBox->setPalette(palette);
    ui->nameEdit->setPalette(palette);
    ui->strasseEdit->setPalette(palette);
    ui->ortEdit->setPalette(palette);
    ui->phoneEdit->setPalette(palette);

}

void FormBetrieb::clearForm()
{
    ui->nrBox->setValue(0);
    ui->nameEdit->clear();
    ui->strasseEdit->clear();
    ui->ortEdit->clear();
    ui->phoneEdit->clear();
    ui->apprenticeTableWidget->clear();
}




