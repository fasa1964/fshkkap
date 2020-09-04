#include "formskills.h"
#include "ui_formskills.h"

#include <QMessageBox>
#include <QPalette>
#include <QDebug>

FormSkills::FormSkills(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSkills)
{
    ui->setupUi(this);

//    ui->tabWidget->setCurrentIndex(0);

    changeSkill = false;
    createSkill = false;
    selectedSkill = ClassSkills();
//    projectFactorChanged = false;

    ui->criteriaBox->addItems(ClassSkills::supportedCriteria());
    setFormReadOnly(true);

    connect(ui->closeButton, &QPushButton::clicked, this, &FormSkills::closeButtonClicked);
    connect(ui->createButton, &QPushButton::clicked, this, &FormSkills::createButtonClicked);
    connect(ui->changeButton, &QPushButton::clicked, this, &FormSkills::changeButtonClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &FormSkills::saveButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &FormSkills::deleteButtonClicked);
//    connect(ui->deleteSkillProjektButton, &QPushButton::clicked, this, &FormSkills::deleteSkillProjektButtonClicked);

//
    connect(ui->importProjekteButton, &QPushButton::clicked, this, &FormSkills::importProjectButtonClicked);
    connect(ui->sortKennunBox, &QComboBox::currentTextChanged, this, &FormSkills::sortProjectBoxTextChanged);
    connect(ui->skillProjektTable, &QTableWidget::itemClicked, this, &FormSkills::skillProjektTableItemClicked);
    connect(ui->projektTable, &QTableWidget::itemClicked, this, &FormSkills::projektTableItemClicked);
    connect(ui->kennungBox, &QComboBox::currentTextChanged, this, &FormSkills::kennungBoxTextChanged);

    connect(ui->projektTable, &QTableWidget::itemClicked, this, &FormSkills::projektTableItemClicked);
    connect(ui->skillTable, &QTableWidget::itemClicked, this, &FormSkills::skillTableItemClicked);
    connect(ui->skillProjektTable, &QTableWidget::cellChanged, this, &FormSkills::skillProjektTableCellClicked);




}

FormSkills::~FormSkills()
{
    delete ui;
}

void FormSkills::closeButtonClicked()
{
    emit formSkillClosed();
    close();
}

void FormSkills::createButtonClicked()
{
    clearForm();
    setFormReadOnly(false);
    setFormTextColor(QColor(0,87,127));

    createSkill = true;
    changeSkill = false;
    selectedSkill = ClassSkills();

    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->nrBox->setFocus();

    ui->countProjekteBox->setValue(0);
    ui->tabWidget->setCurrentIndex(1);

    ui->changeButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    ui->createButton->setEnabled(false);
    ui->saveButton->setEnabled(true);
}

void FormSkills::changeButtonClicked()
{
    setFormReadOnly(false);
    setFormTextColor(QColor(0,87,127));


    createSkill = false;
    changeSkill = true;

    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->nrBox->setFocus();

    ui->tabWidget->setCurrentIndex(1);

    ui->changeButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    ui->createButton->setEnabled(false);
    ui->saveButton->setEnabled(true);
}

void FormSkills::saveButtonClicked()
{

    // Check entered values
    if(ui->nrBox->value() <= 0 || ui->nameEdit->text().isEmpty() || ui->kennungEdit->text().isEmpty() ||
            ui->dateEdit->date() == QDate() || ui->wertBox->value() <= 0 || ui->skillProjektTable->rowCount() == 0)
    {
        QMessageBox::information(this, tr("Prüfung speichern"), tr("Prüfungs-Nr., Name, Kennung, Datum, Wert(Faktor) "
                                           "sowie Zuordnung von Projekten sind erforderlich zum Speichern!"));
        return;
    }

    if(!changeSkill){
        // Check if identifier of skill exist by projects identifiers
        if(!getProjectKennung(projektMap()).contains(ui->kennungEdit->text())){
            QMessageBox::information(this, tr("Prüfung speichern"), tr("Die Kennung einer Prüfung muss mit einer der Kennungen"
                                                                       " von den Projekten übereinstimmen!"));
            return;
        }
    }


    if(!projektFactorSimilarly()){
        double f = getProjectFactor();
        if(f != 1){
            QMessageBox::information(this, tr("Prüfung speichern"), tr("Der Faktor muss ingesamt gleich 1.0 betragen!\n"
                                                                       "Wenn alle Projekte 1.0 betragen, wird die Gewichtung gleichermaßen aufgeteilt!"));
            return;
       }
    }

    ClassSkills skill = readFromForm();

    m_skillMap.insert(skill.getKey(), skill);
    emit saveSkillsMap(skillMap());

    setupSkillTable(skillMap());


    if(changeSkill)
        skillChanged(skill);


    selectedSkill = skill;
    setSkillToForm(selectedSkill);


    ui->changeButton->setEnabled(true);
    ui->deleteButton->setEnabled(true);
    ui->createButton->setEnabled(true);
    ui->saveButton->setEnabled(false);

    createSkill = false;
    changeSkill = false;
    setFormReadOnly(true);
    setFormTextColor(Qt::black);
}

void FormSkills::deleteButtonClicked()
{

    QString message = tr("Die Prüfung: ") + selectedSkill.getKey() + "\n"
                         "wird unwiderruflich gelöscht!";

    int result = QMessageBox::information(this, tr("Prüfung löschen"), message,
                                          QMessageBox::Cancel | QMessageBox::Ok);

    if(result == QMessageBox::Cancel)
        return;

    if(m_skillMap.remove(selectedSkill.getKey()) != 1){
        QMessageBox::information(this, tr("Prüfung löschen"), tr("Das Löschen der Prüfung ist fehlgeschlagen!"));
        return;
    }

    emit saveSkillsMap(m_skillMap);


    int delProjekt = QMessageBox::question(this, tr("Prüfung löschen"), tr("Sollen alle Projekte aus der Prüfung auch gelöscht werden?"),
                                          QMessageBox::No | QMessageBox::Ok);
    if(delProjekt == QMessageBox::Ok)
        emit removeProjects(selectedSkill.getProjektMap());

    clearForm();
    if(!m_skillMap.isEmpty())
        setSkillToForm(m_skillMap.values().first());

    setupSkillTable(m_skillMap);

}

/// !brief Import the selected projects from
/// projectTable into the skillProjectTable
void FormSkills::importProjectButtonClicked()
{
    if(createSkill)
        setupSkillProjectTable( getSelectedProjects() );

    if(changeSkill){
        QMap<QString, ClassProjekt> pMap = getSelectedProjects();
        if(selectedSkill.getProjektMap().size() > 0){
            //QMap<QString, ClassProjekt> spMap = selectedSkill.getProjektMap();
            foreach (ClassProjekt pro, selectedSkill.getProjektMap().values()) {
                pMap.insert(pro.getKey(), pro);

            }
        }

        setupSkillProjectTable(pMap);

    }
}

void FormSkills::skillTableItemClicked(QTableWidgetItem *item)
{
    int row = item->row();
    QString key = ui->skillTable->item(row,1)->text()+"."+ui->skillTable->item(row,2)->text();

    selectedSkill = skillMap().value(key);
    setSkillToForm(selectedSkill);

    setFormReadOnly(true);
    setFormTextColor(Qt::black);

    changeSkill = false;
    createSkill = false;

    ui->changeButton->setEnabled(true);
    ui->deleteButton->setEnabled(true);
    ui->createButton->setEnabled(true);
    ui->saveButton->setEnabled(false);

    ui->deleteSkillProjektButton->setEnabled(false);
    ui->importProjekteButton->setEnabled(false);
}

void FormSkills::projektTableItemClicked(QTableWidgetItem *)
{
    if(!changeSkill && !createSkill)
        return;

    if(ui->sortKennunBox->currentText() == "Alle"){
        if( !isItemChecked(ui->projektTable))
            ui->importProjekteButton->setEnabled(false);
        else
            ui->importProjekteButton->setEnabled(true);
        return;
    }

    if(isItemChecked(ui->projektTable))
        ui->importProjekteButton->setEnabled(true);
    else
        ui->importProjekteButton->setEnabled(false);
}

void FormSkills::skillProjektTableItemClicked(QTableWidgetItem *)
{
    if(!changeSkill && !createSkill)
        return;

    if(isItemChecked(ui->skillProjektTable))
        ui->deleteSkillProjektButton->setEnabled(true);
    else
        ui->deleteSkillProjektButton->setEnabled(false);
}

/// !brief access the project factor
/// while entered values
void FormSkills::skillProjektTableCellClicked(int row, int column)
{
    if(!changeSkill)
        return;

    if(column != 3)
        return;

    QString text = ui->skillProjektTable->item(row, 3)->text();
    foreach (QChar c, text) {
        if(!c.isDigit()){
            if(c != '.'){
                QMessageBox::information(this, tr("Eingabe"), tr("Es dürfen nur Zahlen und als Trennung ein Punkt eingegeben werden!"));
                ui->skillProjektTable->item(row, 3)->setText("1.0");
                return;
            }
        }
    }

    double var = text.toDouble();
    if(var > 1.0){
        QMessageBox::information(this, tr("Eingabe"), tr("Der Faktor darf nicht höher sein als 1.0!"));
        ui->skillProjektTable->item(row, 3)->setText("1.0");
        return;
    }

}

void FormSkills::kennungBoxTextChanged(const QString &text)
{
    if(!changeSkill && !createSkill)
        return;

    ui->kennungEdit->setText(text);
}

//void FormSkills::skillProjektTableCellClicked(int row, int column)
//{
//    if(!changeSkill)
//        return;

//    if(column != 3)
//        return;

//    bool ok = true;
//    QString text = ui->skillProjektTable->item(row, 3)->text();
//    foreach (QChar c, text) {
//        if(!c.isDigit() )
//            ok = false;
//    }

//    if(!ok){
//        QMessageBox::information(this, tr("Eingabe-Fehler"), tr("Bitte nur Zahlen eingeben!") );
//        return;
//    }


//}

void FormSkills::sortProjectBoxTextChanged(const QString &text)
{
    if(text == "Alle"){
        setupProjektTable(projektMap(), Qt::Unchecked);
        ui->importProjekteButton->setEnabled(false);
        return;
    }

    // Sort the project by identifier
    QMap<QString, ClassProjekt> sortMap;
    QMapIterator<QString, ClassProjekt> it(projektMap());
    while (it.hasNext()) {
        it.next();
        ClassProjekt pro = it.value();
        if(pro.identifier() == text)
            sortMap.insert(pro.getKey(), pro);
    }

    if(!sortMap.isEmpty()){
        setupProjektTable(sortMap, Qt::Checked);

        if(createSkill || changeSkill)
            ui->importProjekteButton->setEnabled(true);

    }

}

ClassSkills FormSkills::readFromForm()
{
    ClassSkills skill;

    skill.setNr(ui->nrBox->value());
    skill.setName( ui->nameEdit->text());
    skill.setIdentifier(ui->kennungEdit->text());
    skill.setDate( ui->dateEdit->date() );
    skill.setWert( ui->wertBox->value() );
    skill.setCreatedDate( ui->dateTimeEdit->dateTime() );

    skill.setEvaluationType(ui->criteriaBox->currentIndex());

    skill.setProjektMap( getSkillProjectMap() );

    return skill;
}

/// !brief Returns a list of identifiers
QStringList FormSkills::getProjectKennung(const QMap<QString, ClassProjekt> &pMap)
{
    QStringList kennungList;
    QMapIterator<QString, ClassProjekt> it(pMap);
    while (it.hasNext()) {
        it.next();
        if(!kennungList.contains(it.value().identifier()))
            kennungList << it.value().identifier();
    }

    return kennungList;
}

/// !brief Returns the associated projects from the skill
QMap<QString, ClassProjekt> FormSkills::getSkillProjectMap()
{
    QMap<QString, ClassProjekt> pMap;
    int rows = ui->skillProjektTable->rowCount();
    for(int i = 0; i < rows; i++){
        QString key = ui->skillProjektTable->item(i,1)->text()+"."+ ui->skillProjektTable->item(i,2)->text();
        ClassProjekt pro = projektMap().value(key);

        // set the project factor
        double val = ui->skillProjektTable->item(i,3)->text().toDouble();
        pro.setFactor(val);

        // Check if project is valid
        if(pro.name().isEmpty())
            ; //
        else
           pMap.insert(pro.getKey(), pro);

    }

    return pMap;
}

QMap<QString, ClassProjekt> FormSkills::getSelectedProjects()
{
    QMap<QString, ClassProjekt> pMap;

    int rows = ui->projektTable->rowCount();
    for(int i = 0; i < rows; i++){
        if(ui->projektTable->item(i,6)->checkState() == Qt::Checked){
            QString key = ui->projektTable->item(i,1)->text()+"."+ui->projektTable->item(i,2)->text();
            ClassProjekt pro = m_projektMap.value(key);
            pMap.insert(key, pro);
        }
    }

    return pMap;
}

bool FormSkills::isItemChecked(QTableWidget *widget)
{
    bool status = false;

    for(int i = 0; i < widget->rowCount(); i++){
        if(widget->item(i,6)->checkState() == Qt::Checked){
            status = true;
            break;
        }
    }

    return status;

}

double FormSkills::getProjectFactor()
{
    double value;
    for(int i = 0; i < ui->skillProjektTable->rowCount(); i++){
        double val =  ui->skillProjektTable->item(i,3)->text().toDouble();
        value += val;
    }
    return value;
}


bool FormSkills::projektFactorSimilarly()
{
    bool status = true;
    for(int i = 0; i < ui->skillProjektTable->rowCount(); i++){
        double val = ui->skillProjektTable->item(i,3)->text().toDouble();
        if(val != 1.0){
            status = false;
            break;
        }
    }
    return status;
}

void FormSkills::setupSkillTable(const QMap<QString, ClassSkills> &sMap)
{
    ui->skillTable->clear();
    ui->skillTable->setColumnCount(5);
    ui->skillTable->setRowCount(sMap.size());

    QStringList labels;
    labels << "Nr." << "Name" << "Kennung" << "Datum" << "Projekt";
    ui->skillTable->setHorizontalHeaderLabels(labels);

    int row = 0;
    QMapIterator<QString, ClassSkills> it(sMap);
    while (it.hasNext()) {
        it.next();
        ClassSkills skill = it.value();

        QTableWidgetItem *itemNr = new QTableWidgetItem( QString::number(skill.getNr(),10));
        QTableWidgetItem *itemName = new QTableWidgetItem( skill.name() );
        QTableWidgetItem *itemKennung = new QTableWidgetItem( skill.identifier() );
        QTableWidgetItem *itemDate = new QTableWidgetItem( skill.getCreatedDate().toString("dd.MM.yyyy hh:mm:ss") );
        QTableWidgetItem *itemPro = new QTableWidgetItem( QString::number(skill.getProjektMap().size(),10) );

        ui->skillTable->setItem(row,0, itemNr);
        ui->skillTable->setItem(row,1, itemName);
        ui->skillTable->setItem(row,2, itemKennung);
        ui->skillTable->setItem(row,3, itemDate);
        ui->skillTable->setItem(row,4, itemPro);

        itemNr->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemName->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemKennung->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemDate->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemPro->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

        row++;
    }

    ui->skillTable->resizeColumnToContents(0);
    ui->skillTable->resizeColumnToContents(1);
    ui->skillTable->resizeColumnToContents(2);
    ui->skillTable->resizeColumnToContents(3);
    ui->skillTable->resizeColumnToContents(4);
}

void FormSkills::setupSkillProjectTable(const QMap<QString, ClassProjekt> &pMap)
{
    ui->skillProjektTable->clear();
    ui->skillProjektTable->setColumnCount(7);
    ui->skillProjektTable->setRowCount(pMap.size());

    QStringList labels;
    labels << "Nr." << "Name" << "Kennung" << "Faktor" << "Zeit" << "Datum" << "Marker";
    ui->skillProjektTable->setHorizontalHeaderLabels(labels);

    int row = 0;
    QMapIterator<QString, ClassProjekt> it(pMap);
    while (it.hasNext()) {
        it.next();

        ClassProjekt pro = it.value();

//        QDoubleSpinBox *fac = new QDoubleSpinBox();
//        fac->setRange(0.1 , 1.0);
//        fac->setSingleStep(0.1);
//        fac->setMaximumWidth(90);
//        fac->setValue(pro.getFactor());


        QTableWidgetItem *itemNr = new QTableWidgetItem( QString::number(pro.nr(),10));
        QTableWidgetItem *itemName = new QTableWidgetItem( pro.name() );
        QTableWidgetItem *itemKennung = new QTableWidgetItem( pro.identifier() );
        //QTableWidgetItem *itemFactor = new QTableWidgetItem();
        QTableWidgetItem *itemFactor = new QTableWidgetItem( QString::number(pro.getFactor(),'g',2) );
        QTableWidgetItem *itemMinutes = new QTableWidgetItem( QString::number(pro.getDuration(),10) );
        QTableWidgetItem *itemDate = new QTableWidgetItem( pro.createTime() );
        QTableWidgetItem *itemCheck = new QTableWidgetItem();

        //ui->skillProjektTable->setCellWidget(row, 3, fac);


        ui->skillProjektTable->setItem(row, 0, itemNr);
        ui->skillProjektTable->setItem(row, 1, itemName);
        ui->skillProjektTable->setItem(row, 2, itemKennung);
        ui->skillProjektTable->setItem(row, 3, itemFactor);
        ui->skillProjektTable->setItem(row, 4, itemMinutes);
        ui->skillProjektTable->setItem(row, 5, itemDate);
        ui->skillProjektTable->setItem(row, 6, itemCheck);

        itemFactor->setToolTip(tr("Hier kann der Faktor des Projektes geändert werden.\n"
                                  "Die Änderung wird auch in den Stammdaten des Projektes übertragen.\n"
                                  "Die Faktoren aller Projekte in einer Prüfung muss insgesamt 1.0 betragen!"));

        itemNr->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemName->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemKennung->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemMinutes->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemDate->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);


        itemCheck->setCheckState(Qt::Unchecked);

        row++;
    }

    ui->skillProjektTable->resizeColumnToContents(0);
    ui->skillProjektTable->resizeColumnToContents(1);
    ui->skillProjektTable->resizeColumnToContents(2);
    ui->skillProjektTable->resizeColumnToContents(3);
    ui->skillProjektTable->resizeColumnToContents(4);
    ui->skillProjektTable->resizeColumnToContents(5);
    ui->skillProjektTable->resizeColumnToContents(6);

    ui->countProjekteBox->setValue(pMap.size());
}

void FormSkills::setupProjektTable(const QMap<QString, ClassProjekt> &pMap, Qt::CheckState state)
{
    ui->projektTable->clear();
    ui->projektTable->setColumnCount(7);
    ui->projektTable->setRowCount(pMap.size());

    QStringList labels;
    labels << "Nr." << "Name" << "Kennung" << "Faktor" << "Zeit" << "Datum" << "Marker";
    ui->projektTable->setHorizontalHeaderLabels(labels);

    int row = 0;
    QMapIterator<QString, ClassProjekt> it(pMap);
    while (it.hasNext()) {
        it.next();
        ClassProjekt pro = it.value();
        QTableWidgetItem *itemNr = new QTableWidgetItem( QString::number(pro.nr(),10));
        QTableWidgetItem *itemName = new QTableWidgetItem( pro.name() );
        QTableWidgetItem *itemKennung = new QTableWidgetItem( pro.identifier() );
        QTableWidgetItem *itemFactor = new QTableWidgetItem( QString::number(pro.getFactor(),'g',2) );
        QTableWidgetItem *itemMinutes = new QTableWidgetItem( QString::number(pro.getDuration(), 10) );
        QTableWidgetItem *itemDate = new QTableWidgetItem( pro.createTime() );
        QTableWidgetItem *itemCheck = new QTableWidgetItem();

        ui->projektTable->setItem(row, 0, itemNr);
        ui->projektTable->setItem(row, 1, itemName);
        ui->projektTable->setItem(row, 2, itemKennung);
        ui->projektTable->setItem(row, 3, itemFactor);
        ui->projektTable->setItem(row, 4, itemMinutes);
        ui->projektTable->setItem(row, 5, itemDate);
        ui->projektTable->setItem(row, 6, itemCheck);

        itemCheck->setCheckState(state);

        itemNr->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemName->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
        itemKennung->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemFactor->setFlags(Qt::ItemIsSelectable  | Qt::ItemIsSelectable);
        itemMinutes->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemDate->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

        row++;
    }

    ui->projektTable->resizeColumnToContents(0);
    ui->projektTable->resizeColumnToContents(1);
    ui->projektTable->resizeColumnToContents(2);
    ui->projektTable->resizeColumnToContents(3);
    ui->projektTable->resizeColumnToContents(4);
    ui->projektTable->resizeColumnToContents(5);
    ui->projektTable->resizeColumnToContents(6);
}

void FormSkills::setSkillToForm(const ClassSkills &skill)
{
    ui->nrBox->setValue( skill.getNr());
    ui->nameEdit->setText(skill.name());
    ui->dateEdit->setDate(skill.date());
    ui->kennungEdit->setText(skill.identifier());
    ui->wertBox->setValue(skill.getWert() );
    ui->dateTimeEdit->setDateTime(skill.getCreatedDate());

    ui->kennungBox->setCurrentText( skill.identifier() );
    setupSkillProjectTable(skill.getProjektMap());

    // calculate the duration time of skill
    int min = 0;
    foreach (ClassProjekt p, skill.getProjektMap().values()) {
        min = min + p.getDuration();
    }
    ui->durationBox->setValue(min);

    int index = skill.getEvaluationIndex();
    QString criteriaText = skill.getEvaluationText(index);
    ui->criteriaBox->setCurrentText(criteriaText);

}

QMap<QString, ClassProjekt> FormSkills::projektMap() const
{
    return m_projektMap;
}

void FormSkills::setProjektMap(const QMap<QString, ClassProjekt> &projektMap)
{
    m_projektMap = projektMap;

    // Setup the identifier for sortBox and kennungBox
    ui->kennungBox->clear();
    ui->kennungBox->addItems( getProjectKennung(projektMap) );

    ui->sortKennunBox->clear();
    QStringList sortList;
    sortList << "Alle" << getProjectKennung(projektMap);
    ui->sortKennunBox->addItems( sortList );

    setupProjektTable(projektMap, Qt::Unchecked);

}

QMap<QString, ClassSkills> FormSkills::skillMap() const
{
    return m_skillMap;
}

void FormSkills::setSkillMap(const QMap<QString, ClassSkills> &skillMap)
{
    m_skillMap = skillMap;
    setupSkillTable(skillMap);
}

///// !brief Delete a projekt form skill
//void FormSkills::deleteSkillProjektButtonClicked()
//{
//    int size = ui->skillProjektTable->rowCount();
//    int column = ui->skillProjektTable->columnCount()-1;
//    QStringList keyList;
//    for(int i = 0; i < size; i++)
//    {
//        if(ui->skillProjektTable->item(i, column)->checkState() == Qt::Checked)
//        {
//            QString key = ui->skillProjektTable->item(i, 1)->text() + "." + ui->skillProjektTable->item(i, 2)->text();
//            keyList << key;
//        }
//    }


//    QMap<QString, ClassProjekt> pMap;
//    pMap = selectedSkill.getProjektMap();

//    foreach(QString k, keyList)
//        pMap.remove(k);

//    selectedSkill.setProjektMap(pMap);
//    setSkillProjektToForm(selectedSkill.getProjektMap());

//}



//void FormSkills::deleteButtonClicked()
//{
//    int result = QMessageBox::information(this, tr("Prüfung löschen"), tr("Die Prüfung:\n")+selectedSkill.name()+"-"+selectedSkill.identifier()+
//                                       tr("\nwird unwiderruflich gelöscht!"), QMessageBox::Ok | QMessageBox::Cancel);
//    if(result == QMessageBox::Cancel)
//        return;


//    if(m_skillMap.remove(selectedSkill.getKey()) == 1)
//    {
//        emit saveSkillsMap(m_skillMap);
//        setupSkillTable(m_skillMap);
//        selectedSkill = ClassSkills();
//        clearForm();
//        setFormReadOnly(true);

//        ui->changeButton->setEnabled(false);
//        ui->deleteButton->setEnabled(false);
//        ui->createButton->setEnabled(true);
//        ui->saveButton->setEnabled(false);
//    }
//    else
//    {
//        QMessageBox::information(this, tr("Prüfung löschen"), tr("Die Prüfung:\n")+selectedSkill.name()+"-"+selectedSkill.identifier()+
//                                              tr("\nkonnte nicht gelöscht werden!"), QMessageBox::Ok | QMessageBox::Cancel);

//    }
//}


//void FormSkills::saveButtonClicked()
//{
//    if(ui->nrBox->value() <= 0 || ui->nameEdit->text().isEmpty() || ui->kennungEdit->text().isEmpty() ||
//            ui->dateEdit->date() == QDate() || ui->wertBox->value() <= 0)
//    {
//        QMessageBox::information(this, tr("Prüfung speichern"), tr("Prüfungs-Nr., Name, Kennung und Datum sowie Wert "
//                                           "sind erforderlich zum Speichern!"));
//        return;
//    }

//    if(projectFactorChanged){
//        double factor = getSkillProjectFactor();
//        if(factor > 1.0 || factor < 1.0){
//            QMessageBox::information(this, tr("Eingabe"), tr("Die Faktoren der Projeke müssen insgesamt gleich 1.0 betragen!"));
//            return;
//        }
//    }

//    ClassSkills skill = readFromForm();
//    QString key = skill.getKey();

//    if(projektMap().keys().contains(key))
//    {
//        QMessageBox::information(this, tr("Prüfung speichern"), tr("Die Prüfung existiert bereits!\n"
//                     "Das überschreiben von Prüfungen ist nicht zulässig!\n"));
//        return;
//    }

//    if(skill.getProjektMap().size() <= 0)
//    {
//        QMessageBox::information(this, tr("Prüfung speichern"), tr("Die Prüfung hat keine Projekte!\n"
//                                                                   "Projekte müssen zugeordnet werden!\n"));
//        return;
//    }



//    m_skillMap.insert(key, skill);
//    emit saveSkillsMap( skillMap() );

//    setSkillMap(skillMap());

//    if(changeSkill)
//        emit skillChanged(skill);

//    if(projectFactorChanged){
//        QMapIterator<QString, ClassProjekt> ip(selectedSkill.getProjektMap());
//        while(ip.hasNext()){
//            ip.next();
//            emit skillProjektChanged(ip.value());
//        }
//    }

//    ui->deleteSkillProjektButton->setEnabled(false);
//    ui->importProjekteButton->setEnabled(false);
//    ui->saveButton->setEnabled(false);
//    ui->changeButton->setEnabled(true);
//    ui->deleteButton->setEnabled(true);
//    ui->createButton->setEnabled(true);

//    selectedSkill = skill;
//    setSkillToForm(selectedSkill);

//    changeSkill = false;
//    createSkill = false;
//    projectFactorChanged = false;
//}

//void FormSkills::importProjekteButtonClicked()
//{

//    // Get selected projects
//    QMap<QString, ClassProjekt> proMap;

//    if(selectedSkill.getProjektMap().size() > 0)
//        proMap = selectedSkill.getProjektMap();


//    int size = ui->projektTable->rowCount();
//    for(int i = 0; i < size; i++)
//    {
//        if(ui->projektTable->item(i,3)->checkState() == Qt::Checked){
//            QString key = ui->projektTable->item(i,1)->text()+"."+ui->projektTable->item(i,2)->text();
//            ClassProjekt pro = projektMap().value(key);
//            proMap.insert(key, pro);
//        }
//    }

//    setSkillProjektToForm(proMap);

//    if(changeSkill){
//        setFormReadOnly(false);
//        setFormTextColor(QColor(0,87,127));
//    }

//}

//void FormSkills::dragDropButtonClicked()
//{
//    ui->projektTable->setDragDropMode(QAbstractItemView::DragOnly);
//    ui->skillProjektTable->setDragDropMode(QAbstractItemView::DragDrop);

//}

//void FormSkills::sortKennungBoxTextChanged(const QString &text)
//{

//    if(text == "Alle"){
//        ui->importProjekteButton->setEnabled(false);
//        setupProjektTable(projektMap(), Qt::Unchecked);
//        return;
//    }

//    QMap<QString, ClassProjekt> sortMap;
//    QMapIterator<QString, ClassProjekt> it(projektMap());
//    while (it.hasNext()) {
//        it.next();
//        if(it.value().identifier() == text)
//            sortMap.insert(it.key(), it.value());
//    }

//    if(!sortMap.isEmpty())
//        setupProjektTable(sortMap, Qt::Checked);

//    if(changeSkill || createSkill)
//        ui->importProjekteButton->setEnabled(true);
//}

/// !brief Item has selected
//void FormSkills::projektTableItemClicked(QTableWidgetItem *)
//{
//    if(ui->sortKennunBox->currentText() != "Alle")
//        return;

//    if(isItemChecked(ui->projektTable))
//        ui->importProjekteButton->setEnabled(true);
//    else
//        ui->importProjekteButton->setEnabled(false);
//}


//QMap<QString, ClassSkills> FormSkills::skillMap() const
//{
//    return m_skillMap;
//}

//void FormSkills::setSkillMap(const QMap<QString, ClassSkills> &skillMap)
//{
//    m_skillMap = skillMap;
//    //setupSkillTable(skillMap);
//}

//void FormSkills::skillTableItemClicked(QTableWidgetItem *item)
//{
//    int row = item->row();
//    QString key = ui->skillTable->item(row,1)->text()+"."+ui->skillTable->item(row,2)->text();
//    selectedSkill = skillMap().value(key);
//    setSkillToForm(selectedSkill);
//    setFormReadOnly(true);
//    setFormTextColor(Qt::black);

//    changeSkill = false;
//    createSkill = false;

//    ui->changeButton->setEnabled(true);
//    ui->deleteButton->setEnabled(true);
//    ui->createButton->setEnabled(true);
//    ui->saveButton->setEnabled(false);

//    ui->deleteSkillProjektButton->setEnabled(false);
//    ui->importProjekteButton->setEnabled(false);
//}

//void FormSkills::skillProjektTableItemClicked(QTableWidgetItem *)
//{
//    if(!changeSkill)
//        return;

//    if(isItemChecked(ui->skillProjektTable))
//        ui->deleteSkillProjektButton->setEnabled(true);
//    else
//        ui->deleteSkillProjektButton->setEnabled(false);
//}

//void FormSkills::skillProjektTableCellClicked(int row, int column)
//{
//    if(!changeSkill)
//        return;

//    // Factor has clicked
//    if(column == 3){
//        double fac = ui->skillProjektTable->item(row,3)->text().toDouble();
//        if(fac > 1.0){
//            QMessageBox::information(this, tr("Eingabe"), tr("Der Faktor eines Projektes darf nichT höher sein wie 1.0"));
//            ui->skillProjektTable->item(row,3)->setText("1");
//            return;
//        }

//        projectFactorChanged = true;
//    }
//}

//bool FormSkills::isItemChecked(QTableWidget *widget)
//{
//    bool status = false;
//    int size = widget->rowCount();
//    int column = widget->columnCount()-1;
//    for(int i = 0; i < size; i++){
//        if(widget->item(i,column)->checkState() == Qt::Checked){
//            status = true;
//            break;
//        }
//    }

//    return status;
//}

//void FormSkills::kennungBoxTextChanged(const QString &text)
//{
//    if(createSkill || changeSkill)
//        ui->kennungEdit->setText(text);
//}

//void FormSkills::setupSkillTable(const QMap<QString, ClassSkills> &sMap)
//{
//    ui->skillTable->clear();
//    ui->skillTable->setColumnCount(3);
//    ui->skillTable->setRowCount(sMap.size());

//    QStringList labels;
//    labels << "Nr." << "Name" << "Kennung";
//    ui->skillTable->setHorizontalHeaderLabels(labels);

//    int row = 0;
//    QMapIterator<QString, ClassSkills> it(sMap);
//    while (it.hasNext()) {
//        it.next();
//        ClassSkills skill = it.value();

//        QTableWidgetItem *itemNr = new QTableWidgetItem( QString::number(skill.getNr(),10));
//        QTableWidgetItem *itemName = new QTableWidgetItem( skill.name() );
//        QTableWidgetItem *itemKennung = new QTableWidgetItem( skill.identifier() );

//        ui->skillTable->setItem(row,0, itemNr);
//        ui->skillTable->setItem(row,1, itemName);
//        ui->skillTable->setItem(row,2, itemKennung);

//        itemNr->setFlags(Qt::ItemIsEnabled);
//        itemName->setFlags(Qt::ItemIsEnabled);
//        itemKennung->setFlags(Qt::ItemIsEnabled);

//        row++;
//    }

//    ui->skillTable->resizeColumnToContents(0);
//    ui->skillTable->resizeColumnToContents(1);
//    ui->skillTable->resizeColumnToContents(2);
//}

//void FormSkills::setSkillToForm(const ClassSkills &skill)
//{
//    ui->nrBox->setValue( skill.getNr());
//    ui->nameEdit->setText(skill.name());
//    ui->dateEdit->setDate(skill.date());
//    ui->kennungEdit->setText(skill.identifier());
//    ui->wertBox->setValue(skill.getWert() );
//    ui->dateTimeEdit->setDateTime(skill.getCreatedDate());
//    setSkillProjektToForm(skill.getProjektMap());

//    int min = 0;
//    foreach (ClassProjekt p, skill.getProjektMap().values()) {
//        min = min + p.getDuration();
//    }

//    ui->durationBox->setValue(min);

//    ui->criteriaBox->setCurrentIndex(skill.getEvaluationType());
//}


//void FormSkills::setSkillProjektToForm(const QMap<QString, ClassProjekt> &proMap)
//{
//    ui->skillProjektTable->clear();
//    ui->skillProjektTable->setColumnCount(6);
//    ui->skillProjektTable->setRowCount(proMap.size());

//    QStringList labels;
//    labels << "Nr." << "Name" << "Kennung" << "Faktor" << "Marker" << "Datum";
//    ui->skillProjektTable->setHorizontalHeaderLabels(labels);

//    int row = 0;
//    QMapIterator<QString, ClassProjekt> it(proMap);
//    while (it.hasNext()) {
//        it.next();
//        ClassProjekt pro = it.value();
//        QTableWidgetItem *itemNr = new QTableWidgetItem( QString::number(pro.nr(),10));
//        QTableWidgetItem *itemName = new QTableWidgetItem( pro.name() );
//        QTableWidgetItem *itemKennung = new QTableWidgetItem( pro.identifier() );
//        QTableWidgetItem *itemFactor = new QTableWidgetItem( QString::number(pro.getFactor(),'g',2) );
//        QTableWidgetItem *itemCheck = new QTableWidgetItem();
//        QTableWidgetItem *itemDate = new QTableWidgetItem( pro.createTime() );

//        ui->skillProjektTable->setItem(row, 0, itemNr);
//        ui->skillProjektTable->setItem(row, 1, itemName);
//        ui->skillProjektTable->setItem(row, 2, itemKennung);
//        ui->skillProjektTable->setItem(row, 3, itemFactor);
//        ui->skillProjektTable->setItem(row, 4, itemCheck);
//        ui->skillProjektTable->setItem(row, 5, itemDate);

//        itemFactor->setToolTip(tr("Hier kann der Faktor des Projektes geändert werden.\n"
//                                  "Die Änderung wird auch in den Stammdaten des Projektes übertragen.\n"
//                                  "Die Faktoren aller Projekte in einer Prüfung muss insgesamt 1.0 betragen!"));

//        itemNr->setFlags(Qt::ItemIsEnabled);
//        itemName->setFlags(Qt::ItemIsEnabled);
//        itemKennung->setFlags(Qt::ItemIsEnabled);
//        itemDate->setFlags(Qt::ItemIsEnabled);
//        itemCheck->setCheckState(Qt::Unchecked);

//        row++;
//    }

//    ui->skillProjektTable->resizeColumnToContents(0);
//    ui->skillProjektTable->resizeColumnToContents(1);
//    ui->skillProjektTable->resizeColumnToContents(2);
//    ui->skillProjektTable->resizeColumnToContents(3);
//    ui->skillProjektTable->resizeColumnToContents(5);

//    ui->countProjekteBox->setValue(proMap.size());
//}

//ClassSkills FormSkills::readFromForm()
//{
//    ClassSkills skill;

//    skill.setNr(ui->nrBox->value());
//    skill.setName( ui->nameEdit->text());
//    skill.setIdentifier(ui->kennungEdit->text());
//    skill.setDate( ui->dateEdit->date() );
//    skill.setWert( ui->wertBox->value() );
//    skill.setCreatedDate( ui->dateTimeEdit->dateTime() );

//    skill.setProjektMap( getSkillProjektMap() );


//    return skill;
//}

//QMap<QString, ClassProjekt> FormSkills::getSkillProjektMap()
//{
//    QMap<QString, ClassProjekt> map;
//    int size = ui->skillProjektTable->rowCount();
//    for(int i = 0; i < size; i++)
//    {
//        QString key = ui->skillProjektTable->item(i,1)->text()+"."+ui->skillProjektTable->item(i,2)->text();
//        ClassProjekt pro = projektMap().value(key);

//        if(projectFactorChanged){
//            double fac =  ui->skillProjektTable->item(i,3)->text().toDouble();
//            pro.setFactor(fac);
//        }


//        map.insert(key, pro);
//        //map.insert(key, projektMap().value(key));
//    }

//    return map;
//}

//double FormSkills::totalProjectFactor(const ClassSkills &skill)
//{
//    double factor = 0.0;
//    QMap<QString, ClassProjekt> pMap;
//    pMap = skill.getProjektMap();
//    QMapIterator<QString, ClassProjekt> it(pMap);
//    while (it.hasNext()) {
//        it.next();
//        factor += it.value().getFactor();

//    }
//    return factor;
//}



//void FormSkills::setupProjektTable(const QMap<QString, ClassProjekt> &proMap, Qt::CheckState state)
//{
//    ui->projektTable->clear();
//    ui->projektTable->setColumnCount(6);
//    ui->projektTable->setRowCount(proMap.size());

//    QStringList labels;
//    labels << "Nr." << "Name" << "Kennung" << "Marker" << "Zeit" << "Datum";
//    ui->projektTable->setHorizontalHeaderLabels(labels);

//    int row = 0;
//    QMapIterator<QString, ClassProjekt> it(proMap);
//    while (it.hasNext()) {
//        it.next();
//        ClassProjekt pro = it.value();
//        QTableWidgetItem *itemNr = new QTableWidgetItem( QString::number(pro.nr(),10));
//        QTableWidgetItem *itemName = new QTableWidgetItem( pro.name() );
//        QTableWidgetItem *itemKennung = new QTableWidgetItem( pro.identifier() );
//        QTableWidgetItem *itemCheck = new QTableWidgetItem();
//        QTableWidgetItem *itemMinutes = new QTableWidgetItem( QString::number(pro.getDuration(),10) );
//        QTableWidgetItem *itemDate = new QTableWidgetItem( pro.createTime());

//        ui->projektTable->setItem(row, 0, itemNr);
//        ui->projektTable->setItem(row, 1, itemName);
//        ui->projektTable->setItem(row, 2, itemKennung);
//        ui->projektTable->setItem(row, 3, itemCheck);
//        ui->projektTable->setItem(row, 4, itemMinutes);
//        ui->projektTable->setItem(row, 5, itemDate);

//        itemCheck->setCheckState(state);

//        itemNr->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
//        itemName->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled);
//        itemKennung->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
//        //itemCheck->setFlags(Qt::ItemIsSelectable);
//        itemMinutes->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
//        itemDate->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

//        row++;
//    }

//    ui->projektTable->resizeColumnToContents(0);
//    ui->projektTable->resizeColumnToContents(1);
//    ui->projektTable->resizeColumnToContents(2);
//    ui->projektTable->resizeColumnToContents(3);
//    ui->projektTable->resizeColumnToContents(4);
//    ui->projektTable->resizeColumnToContents(5);
//}

//void FormSkills::setupProjektIconTable(const QMap<QString, ClassProjekt> &proMap, Qt::CheckState state)
//{
//    ui->projektTable->clear();
//    ui->projektTable->setColumnCount(5);
//    ui->projektTable->setRowCount(proMap.size());

//    QMapIterator<QString, ClassProjekt> it(proMap);
//    while (it.hasNext()) {
//        it.next();
//        ClassProjekt pro = it.value();
//        QTableWidgetItem *item = new QTableWidgetItem( pro.getKey() );
//        item->setIcon(QIcon(":/ProjectIcon.png"));

//        int row = ui->projektTable->rowCount();
//        ui->projektTable->insertRow(row);
//        ui->projektTable->setItem(row, 0, item);

//    }
//}

//double FormSkills::getSkillProjectFactor()
//{
//    double factor = 0.0;
//    int size = ui->skillProjektTable->rowCount();
//    for(int i = 0; i < size; i++)
//    {
//        factor = factor + ui->skillProjektTable->item(i,3)->text().toDouble();
//    }

//    return factor;
//}

void FormSkills::setFormTextColor(QColor color)
{
    QPalette palette;
    palette.setColor(QPalette::Base,Qt::white);
    palette.setColor(QPalette::Text,color);

    ui->nrBox->setPalette(palette);
    ui->nameEdit->setPalette(palette);
    ui->kennungEdit->setPalette(palette);
    ui->dateEdit->setPalette(palette);
    ui->dateTimeEdit->setPalette(palette);
    ui->wertBox->setPalette(palette);

    int size = ui->skillProjektTable->rowCount();
    for(int i = 0; i < size; i++)
    {
        ui->skillProjektTable->item(i,0)->setTextColor(color);
        ui->skillProjektTable->item(i,1)->setTextColor(color);
        ui->skillProjektTable->item(i,2)->setTextColor(color);
        ui->skillProjektTable->item(i,3)->setTextColor(color);
    }
}

void FormSkills::setFormReadOnly(bool status)
{
    ui->nrBox->setReadOnly(status);
    ui->nameEdit->setReadOnly(status);
    ui->dateEdit->setReadOnly(status);
    ui->kennungEdit->setReadOnly(status);
    ui->kennungBox->setEnabled(!status);
    ui->criteriaBox->setEnabled(!status);
}

void FormSkills::clearForm()
{
    ui->nrBox->setValue(0);
    ui->nameEdit->clear();
    ui->dateEdit->setDate(QDate());
    ui->kennungEdit->clear();
    ui->wertBox->setValue(0);
    ui->durationBox->setValue(0);

    ui->skillProjektTable->setRowCount(0);
    ui->skillProjektTable->clear();
}
