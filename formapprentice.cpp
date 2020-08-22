#include "formapprentice.h"
#include "ui_formapprentice.h"

#include <QMessageBox>
#include <QDebug>

FormLehrling::FormLehrling(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormLehrling)
{
    ui->setupUi(this);

    changeData = false;
    seletedApprentice = ClassLehrling();
    setFormReadOnly(true);
    setupKlasseMap();
    ui->klasseBox->addItems(klasseMap.keys());

    ui->tabWidget->setCurrentIndex(0);

    connect(ui->closeButton, &QPushButton::clicked, this, &FormLehrling::closeButtonClicked);
    connect(ui->createButton, &QPushButton::clicked, this, &FormLehrling::createButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &FormLehrling::deleteButtonClicked);
    connect(ui->deleteSkillButton, &QPushButton::clicked, this, &FormLehrling::deleteSkillButtonClicked);
    connect(ui->changeButton, &QPushButton::clicked, this, &FormLehrling::changeButtonClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &FormLehrling::saveButtonClicked);

    connect(ui->klasseBox, &QComboBox::currentTextChanged, this, &FormLehrling::klasseBoxTextChanged);

}

FormLehrling::~FormLehrling()
{
    delete ui;
}

void FormLehrling::closeButtonClicked()
{
    emit apprenticeFormClosed();
    close();
}


void FormLehrling::createButtonClicked()
{
    clearForm();
    setFormReadOnly(false);
    ui->nrBox->setFocus();
}

void FormLehrling::deleteButtonClicked()
{

}

void FormLehrling::deleteSkillButtonClicked()
{
    QMap<QString, ClassSkills> sMap = seletedApprentice.getSkillMap();
    QString key = ui->exameBox->currentText();

    int result = QMessageBox::information(this, tr("Lösche Prüfung"), tr("Die Prüfung: ") +
                  key + tr("\nwird unwiderruflich gelöscht!"), QMessageBox::Cancel | QMessageBox::Ok);

    if(result == QMessageBox::Cancel)
        return;

    if(sMap.remove(key) == 0)
        QMessageBox::information(this, tr("Lösche Prüfung"), tr("Die Prüfung: ") +
                          key + tr("\nkonnte nicht gelöscht werden!"));

    seletedApprentice = readFromForm();
    seletedApprentice.setSkillMap(sMap);
    setApprenticeToForm(seletedApprentice);

}

void FormLehrling::changeButtonClicked()
{
    setFormReadOnly(false);
    ui->nrBox->setFocus();

    if(seletedApprentice.getSkillMap().size() > 0)
        ui->deleteSkillButton->setEnabled(true);
    else
        ui->deleteSkillButton->setEnabled(false);

    changeData = true;
    ui->changeButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    ui->createButton->setEnabled(false);
    ui->saveButton->setEnabled(true);

}

void FormLehrling::saveButtonClicked()
{
    ClassLehrling appr = readFromForm();

    if(changeData){
        appr.setSkillMap(seletedApprentice.getSkillMap());
    }

    m_apprenticeMap.insert(appr.getKey(), appr);
    emit saveApprenticeMap(m_apprenticeMap);
    changeData = false;

    seletedApprentice = appr;
    setApprenticeToForm(seletedApprentice);
    sortApprenticeTable();

    ui->changeButton->setEnabled(true);
    ui->deleteButton->setEnabled(true);
    ui->createButton->setEnabled(true);
    ui->saveButton->setEnabled(false);
}

void FormLehrling::klasseBoxTextChanged(const QString &text)
{
    ui->klasseEdit->setText(text);
    ui->educationDateEdit->setDate(klasseMap.value(text).toDate());
}

void FormLehrling::apprenticeTableClicked(QTableWidgetItem *item)
{
    QString key;
    int index = ui->tabWidget->currentIndex();
    if(index == 0)
        key = ui->apprenticeTableWidget->item(item->row(),1)->text();
    if(index == 1)
        key = ui->app1Table->item(item->row(),1)->text();
    if(index == 2)
        key = ui->app2Table->item(item->row(),1)->text();
    if(index == 3)
        key = ui->app3Table->item(item->row(),1)->text();
    if(index == 4)
        key = ui->app4Table->item(item->row(),1)->text();
    if(index == 5)
        key = ui->app5Table->item(item->row(),1)->text();

    clearForm();
    seletedApprentice = m_apprenticeMap.value(key);
    setApprenticeToForm(seletedApprentice);
    setFormReadOnly(true);

    changeData = false;
    ui->changeButton->setEnabled(true);
    ui->deleteButton->setEnabled(true);
    ui->createButton->setEnabled(true);
    ui->saveButton->setEnabled(false);
    ui->deleteSkillButton->setEnabled(false);

}

void FormLehrling::sortApprenticeTable()
{

    updateApprenticeTable(ui->apprenticeTableWidget, apprenticeMap());

    for(int i = 1; i < 6; i++)
    {
        QMap<QString, ClassLehrling> sortMap;
        sortMap = getApprenticeMap(i);
        ui->tabWidget->setTabEnabled(i,false);

        if(i == 1 && !sortMap.isEmpty()){
            ui->tabWidget->setTabEnabled(i,true);
            updateApprenticeTable(ui->app1Table, sortMap);
        }
        if(i == 2 && !sortMap.isEmpty()){
            ui->tabWidget->setTabEnabled(i,true);
            updateApprenticeTable(ui->app2Table, sortMap);
        }
        if(i == 3 && !sortMap.isEmpty()){
            ui->tabWidget->setTabEnabled(i,true);
            updateApprenticeTable(ui->app3Table, sortMap);
        }

        if(i == 4 && !sortMap.isEmpty()){
            ui->tabWidget->setTabEnabled(i,true);
            updateApprenticeTable(ui->app4Table, sortMap);
        }
        if(i == 5 && !sortMap.isEmpty()){
            ui->tabWidget->setTabEnabled(i,true);
            updateApprenticeTable(ui->app5Table, sortMap);
        }
    }
}

QMap<QString, ClassLehrling> FormLehrling::getApprenticeMap(int year)
{
    int todayYear = QDate::currentDate().year();

    QMap<QString, ClassLehrling> sortMap;
    QMapIterator<QString, ClassLehrling> it(apprenticeMap());
    while (it.hasNext()) {
        it.next();
        int educationYear = todayYear - it.value().apprenticeshipDate().year();

        if(educationYear == 0)
            educationYear = 1;

        if(educationYear > 4)
            educationYear = 5;

        if(educationYear == year)
            sortMap.insert(it.value().getKey(), it.value());

    }

    return sortMap;
}

void FormLehrling::updateApprenticeTable(QTableWidget *widget, const QMap<QString, ClassLehrling> &aMap)
{

    widget->clear();
    widget->setRowCount(aMap.size());
    widget->setColumnCount(4);

    QStringList labels;
    labels << "Nr." << "Name" << "Klasse" << "Betrieb";
    widget->setHorizontalHeaderLabels(labels);

    int row = 0;
    QMapIterator<QString, ClassLehrling> it(aMap);
    while (it.hasNext()) {
        it.next();
        ClassLehrling appr = it.value();

        QTableWidgetItem *itemNr = new QTableWidgetItem( QString::number(appr.nr(),10));
        QTableWidgetItem *itemName = new QTableWidgetItem( it.key() );
        QTableWidgetItem *itemKlasse = new QTableWidgetItem( appr.educationClass() );
        QTableWidgetItem *itemBetrieb = new QTableWidgetItem( appr.company() );

        widget->setItem(row,0,itemNr);
        widget->setItem(row,1,itemName);
        widget->setItem(row,2,itemKlasse);
        widget->setItem(row,3,itemBetrieb);

        itemNr->setTextColor(QColor(0,85,127,255));
        itemNr->setToolTip(tr("Prüfungsnummer"));
        itemName->setTextColor(QColor(0,85,127,255));

        itemNr->setFlags(Qt::ItemIsEnabled);
        itemName->setFlags(Qt::ItemIsEnabled);
        itemKlasse->setFlags(Qt::ItemIsEnabled);
        itemBetrieb->setFlags(Qt::ItemIsEnabled);

        row++;

    }

    widget->resizeColumnToContents(0);
    widget->resizeColumnToContents(1);
    widget->resizeColumnToContents(2);
    widget->resizeColumnToContents(4);

    connect(widget, &QTableWidget::itemClicked, this, &FormLehrling::apprenticeTableClicked);
}

QMap<QString, ClassLehrling> FormLehrling::apprenticeMap() const
{
    return m_apprenticeMap;
}

void FormLehrling::setApprenticeMap(const QMap<QString, ClassLehrling> &apprenticeMap)
{
    m_apprenticeMap = apprenticeMap;
    sortApprenticeTable();
}

void FormLehrling::setFormReadOnly(bool status)
{
    ui->nrBox->setReadOnly(status);
    ui->firstnameEdit->setReadOnly(status);
    ui->nachnameEdit->setReadOnly(status);
    ui->educationDateEdit->setReadOnly(status);
    ui->strasseEdit->setReadOnly(status);
    ui->ortEdit->setReadOnly(status);
    ui->phoneEdit->setReadOnly(status);
    ui->gebDateEdit->setReadOnly(status);
    ui->notizEdit->setReadOnly(status);
    ui->klasseEdit->setReadOnly(status);
    ui->klasseBox->setEnabled(!status);

    ui->betriebEdit->setReadOnly(true);
    ui->betriebButton->setEnabled(!status);

    if(changeData && !status)
        ui->betriebEdit->setReadOnly(false);

    if(!status)
        setFormTextColor(QColor(0,85,127));
    else
        setFormTextColor(Qt::black);

}

void FormLehrling::setApprenticeToForm(const ClassLehrling &appr)
{
    ui->exameBox->clear();
    ui->nrBox->setValue(appr.nr());
    ui->firstnameEdit->setText(appr.firstname());
    ui->nachnameEdit->setText(appr.surname());
    ui->educationDateEdit->setDate(appr.apprenticeshipDate());
    ui->strasseEdit->setText(appr.street());
    ui->ortEdit->setText(appr.city());
    ui->klasseEdit->setText( appr.educationClass());
    ui->phoneEdit->setText(appr.phone());
    ui->betriebEdit->setText( appr.company());
    ui->gebDateEdit->setDate(appr.birthDate());
    ui->notizEdit->setPlainText( appr.note());

    if(appr.getSkillMap().size() > 0){
        ui->exameBox->addItems(appr.getSkillMap().keys());
    }
}

ClassLehrling FormLehrling::readFromForm()
{

    ClassLehrling apprentice;

    apprentice.setNr( ui->nrBox->value() );
    apprentice.setApprenticeshipDate( ui->educationDateEdit->date() );
    apprentice.setFirstname( ui->firstnameEdit->text() );
    apprentice.setSurname( ui->nachnameEdit->text() );
    apprentice.setStreet( ui->strasseEdit->text() );
    apprentice.setCity( ui->ortEdit->text() );
    apprentice.setPhone( ui->phoneEdit->text() );
    apprentice.setEducationClass( ui->klasseEdit->text());
    apprentice.setBirthDate( ui->gebDateEdit->date() );
    apprentice.setCompany( ui->betriebEdit->text() );
    apprentice.setNote( ui->notizEdit->toPlainText());

    return apprentice;

}

void FormLehrling::setFormTextColor(QColor color)
{
    QPalette palette;
    palette.setColor(QPalette::Base,Qt::white);
    palette.setColor(QPalette::Text,color);
    palette.setColor(QPalette::ButtonText,color);

    ui->nrBox->setPalette(palette);
    ui->firstnameEdit->setPalette(palette);
    ui->nachnameEdit->setPalette(palette);
    ui->educationDateEdit->setPalette(palette);
    ui->strasseEdit->setPalette(palette);
    ui->ortEdit->setPalette(palette);
    ui->phoneEdit->setPalette(palette);
    ui->gebDateEdit->setPalette(palette);
    ui->notizEdit->setPalette(palette);
    ui->klasseEdit->setPalette(palette);
    ui->betriebEdit->setPalette(palette);
    ui->exameBox->setPalette(palette);
}

void FormLehrling::clearForm()
{
    ui->nrBox->setValue(0);
    ui->firstnameEdit->clear();
    ui->nachnameEdit->clear();
    ui->educationDateEdit->setDate( QDate() );
    ui->strasseEdit->clear();
    ui->ortEdit->clear();
    ui->phoneEdit->clear();
    ui->betriebEdit->clear();
    ui->gebDateEdit->setDate( QDate());
    ui->notizEdit->clear();
    ui->exameBox->clear();
}

void FormLehrling::setupKlasseMap()
{
    klasseMap.insert("MAS 16/1", QDate(2016,8,1));
    klasseMap.insert("MAS 17/1", QDate(2017,8,1));
    klasseMap.insert("MAS 18/1", QDate(2018,8,1));
    klasseMap.insert("MAS 19/1", QDate(2019,8,1));
    klasseMap.insert("MAS 20/1", QDate(2020,8,1));
    klasseMap.insert("MAS 21/1", QDate(2021,8,1));
    klasseMap.insert("MAS 22/1", QDate(2022,8,1));
    klasseMap.insert("MAS 23/1", QDate(2023,8,1));
    klasseMap.insert("MAS 24/1", QDate(2024,8,1));
    klasseMap.insert("MAS 25/1", QDate(2025,8,1));
    klasseMap.insert("MAS 26/1", QDate(2026,8,1));
    klasseMap.insert("MAS 27/1", QDate(2027,8,1));
    klasseMap.insert("MAS 28/1", QDate(2028,8,1));
    klasseMap.insert("MAS 29/1", QDate(2029,8,1));
    klasseMap.insert("MAS 30/1", QDate(2030,8,1));
}





