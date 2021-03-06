#include "formevaluation.h"
#include "ui_formevaluation.h"

#include <QDate>
#include <QDateTime>
#include <QPalette>
#include <QMessageBox>
#include <QtMath>
#include <QTreeWidgetItem>

#include <fshkwindow.h>
#include <QDebug>

FormEvaluation::FormEvaluation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormEvaluation)
{
    ui->setupUi(this);

    ui->frameAttention->hide();

    dirty = false;
    selectedApprentice = ClassLehrling();
    selectedSkill = ClassSkills();
    selectedProjekt = ClassProjekt();


    apprenticeTableList = new FormApprenticeResultList(0);
    connect(apprenticeTableList, &FormApprenticeResultList::printResultList, this, &FormEvaluation::printButtonClicked);

    connect(ui->closeButton, &QPushButton::clicked, this, &FormEvaluation::closeButtonClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &FormEvaluation::saveButtonClicked);
    connect(ui->listButton, &QPushButton::clicked, this, &FormEvaluation::listButtonClicked);
    connect(ui->azubiSortBox, &QComboBox::currentTextChanged, this, &FormEvaluation::azubiSortBoxChanged);
    connect(ui->azubiListBox, &QComboBox::currentTextChanged, this, &FormEvaluation::azubiListBoxChanged);
    connect(ui->skillListBox, &QComboBox::currentTextChanged, this, &FormEvaluation::azubiSkillBoxChanged);
    connect(ui->projektListBox, &QComboBox::currentTextChanged, this, &FormEvaluation::azubiProjectBoxChanged);
    connect(ui->resultTreeWidget, &QTreeWidget::itemClicked, this, &FormEvaluation::resultTableItemClicked);
    connect(ui->evaluatedCheckBox, &QCheckBox::stateChanged, this, &FormEvaluation::evaluatedCheckBoxChanged);



//
//    connect(ui->recoverButton, &QPushButton::clicked, this, &FormEvaluation::recoverButtonClicked);

//    connect(ui->skillListBox, &QComboBox::currentTextChanged, this, &FormEvaluation::skillBoxTextChanged);

//    connect(ui->resultTreeWidget, &QTreeWidget::itemClicked, this, &FormEvaluation::resultTableItemClicked);
//    connect(ui->factorBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
//          [=](double d){ factorBoxChanged(d); });

}

FormEvaluation::~FormEvaluation()
{
    delete ui;
}


//void FormEvaluation::projectBoxTextChanged(const QString &text)
//{
//    QMap<QString, ClassProjekt> pMap;
//    pMap = selectedSkill.getProjektMap();
//    selectedProjekt = pMap.value(text);

//    ui->projektNameEdit->setText(selectedProjekt.name());
//    ui->evaluatedCheckBox->setChecked(selectedProjekt.getEvaluated());

//    ui->factorBox->setValue( selectedProjekt.getFactor() );
//    if(selectedProjekt.getEvaluated())
//        ui->factorBox->setEnabled(false);
//    else
//        ui->factorBox->setEnabled(true);

//    double percent = selectedProjekt.percent();
//    if(percent < 50)
//        setTextColor(ui->percentBox, Qt::red);
//    else
//        setTextColor(ui->percentBox, Qt::darkGreen);
//    ui->percentBox->setValue(percent);

//    setupQuestionTable(selectedProjekt);

//    //qDebug() << selectedProjekt.createTime();
//}

//void FormEvaluation::evaluatedCheckBoxChanged(int status)
//{

//    if(status == Qt::Checked && selectedProjekt.getEvaluated() != true){
//        selectedProjekt.setEvaluated(true);
//        ui->factorBox->setEnabled(false);
//        setupProjectValue();
//    }

//    if(status == Qt::Unchecked  && selectedProjekt.getEvaluated() == true){
//        selectedProjekt.setEvaluated(false);
//        ui->factorBox->setEnabled(true);
//        setupProjectValue();
//    }

//}

/// !brief Calculate the points of current project
/// when value from questions changed
//void FormEvaluation::questionTableCellChanged(int row, int column)
//{
//    // Check if already evaluated
//    if(ui->evaluatedCheckBox->isChecked()){
//        QMessageBox::information(this, tr("Eingabe"), tr("Das Projekt wurde bereits ausgewertet!\n"
//                                           "Sollte dennoch Änderungen gemacht werden,\n"
//                                           "deaktivieren sie bitte Ausgewertet!"));
//        setupQuestionTable(selectedProjekt);

//        return;
//    }

//    // Check entered value of points
//    QString s = ui->fragenTableWidget->item(row, column)->text();
//    foreach (QChar ch, s) {
//        if(!ch.isDigit()){
//            QMessageBox::information(this, tr("Eingabe"), tr("Bitte nur Zahlen eingeben!"));
//            ui->fragenTableWidget->item(row, column)->setText("0");
//        }
//    }

//    // Check points
//    int points =  s.toInt();
//    int maxPoints = ui->fragenTableWidget->item(row, column+1)->text().toInt();
//    if(points > maxPoints){
//        QMessageBox::information(this, tr("Eingabe"), tr("Der Zahlenwert darf das Maximum nicht überschreiten!"));
//        ui->fragenTableWidget->item(row, column)->setText("0");
//    }

    // Update selected project question map
//    QMap<int, ClassFrage> fMap;
//    fMap = selectedProjekt.questionMap();
//    for(int i = 0; i < ui->fragenTableWidget->rowCount(); i++){
//        int p = ui->fragenTableWidget->item(i, 2)->text().toInt();
//        selectedProjekt.setQuestionPoints(i, p);
//        ClassFrage frg = fMap.value(i);
//        frg.setPoints(p);
//        fMap.insert(i, frg);
//    }
//    selectedProjekt.setQuestionMap(fMap);

//    double pp = selectedProjekt.percent();

//    ui->percentBox->setValue(pp);
//    if(pp < 50.0)
//        setTextColor(ui->percentBox, Qt::red);
//    else
//        setTextColor(ui->percentBox, Qt::darkGreen);

//    setupProjectValue();

//    qDebug() << "Aszubi: " << apprenticePercent(selectedApprentice);
//}

/// !brief
//void FormEvaluation::resultTableItemClicked(QTreeWidgetItem *item, int column)
//{
//    QString key = item->text(column);

//    // Check if key is top item
//    if(selectedApprentice.getSkillMap().keys().contains(key)){
//        // Check if topItem == selsectedSkill
//        if(ui->skillListBox->currentText() != key)
//            ui->skillListBox->setCurrentText(key);
//    }

//    ui->projektListBox->setCurrentText(key);
//}

//void FormEvaluation::factorBoxChanged(double value)
//{
//    if(ui->evaluatedCheckBox->isChecked())
//        return;

//    if(value == selectedProjekt.getFactor())
//        return;

//    selectedProjekt.setFactor(value);
//    selectedSkill.insertProjekt(selectedProjekt);
//    selectedApprentice.insertSkill(selectedSkill);
//    setupResultWidget(selectedApprentice);

//}

// Test function
//double FormEvaluation::apprenticePercent(const ClassLehrling &appr)
//{
//    double percent = 0.0;
//    foreach (ClassSkills skill, appr.getSkillMap().values()) {
//        percent += skillPercent(skill);
//    }

//    return percent;
//}

//double FormEvaluation::skillPercent(const ClassSkills &skill)
//{
//    double percent = 0.0;
//    foreach (ClassProjekt p, skill.getProjektMap().values()) {
//        percent += projectPercent(p) * p.getFactor();
//    }

//    return percent;
//}

//double FormEvaluation::projectPercent(const ClassProjekt &pro)
//{
//    double percent = 0.0;
//    int points = 0;
//    foreach (ClassFrage q, pro.questionMap().values()) {
//        points += q.points();
//    }

//    return percent = points * 100.0 / pro.maxPoints();
//}

QMap<QString, ClassLehrling> FormEvaluation::getApprenticeMap() const
{
    return m_apprenticeMap;
}

void FormEvaluation::setApprenticeMap(const QMap<QString, ClassLehrling> &apprenticeMap)
{
    m_apprenticeMap = apprenticeMap;
    setupApprYearBox();
    updateResultTreeWidget(selectedApprentice);
}

//void FormEvaluation::setRecoverButtonEnable(bool status, const QStringList &skillKeys)
//{
//    ui->recoverButton->setEnabled(status);

//    if(status){
//        QString message = "Folgende Prüfungen wurden gelöscht:\n";
//        for(int i = 0; i < skillKeys.size(); i++){
//            message.append(skillKeys.at(i));
//            message.append("\n\n");
//        }
//        message.append("Die Prüfung/en und die Projekte befinden sich nicht mehr in den Basisdaten.\n"
//                       "Durch betätigen des Wiederherstellungs-Button werden die Prüfungen wieder hergestellt!");
//        QMessageBox::information(this, tr("Wiederherstellung"), message);
//    }

//}

//void FormEvaluation::setupProjectValue()
//{
//    // Update selected skill and selected Lehrling
//    selectedSkill.insertProjekt(selectedProjekt);
//    selectedApprentice.insertSkill(selectedSkill);
//    m_apprenticeMap.insert(selectedApprentice.getKey(), selectedApprentice);

//    setupResultWidget(selectedApprentice);

//    dirty = true;
//    ui->saveButton->setEnabled(true);
//}

//void FormEvaluation::setupQuestionTable(ClassProjekt pro)
//{

//    disconnect(ui->fragenTableWidget, &QTableWidget::cellChanged, this, &FormEvaluation::questionTableCellChanged);

//    ui->fragenTableWidget->clear();
//    ui->fragenTableWidget->setColumnCount(5);
//    ui->fragenTableWidget->setRowCount(pro.questionMap().size());

//    QStringList labels;
//    labels << "Nr." << "Frage" << "Punkte" << "Max. Punkte" << "Kennung" ;
//    ui->fragenTableWidget->setHorizontalHeaderLabels(labels);

//    QMap<int, ClassFrage> fMap = pro.questionMap();

//    int row = 0;
//    QMapIterator<int, ClassFrage> it(fMap);
//    while (it.hasNext()) {
//        it.next();

//        ClassFrage question = it.value();
//        QTableWidgetItem *itemNr = new QTableWidgetItem( QString::number(question.questionNr(),10) );
//        QTableWidgetItem *itemFrage = new QTableWidgetItem( question.question() );
//        QTableWidgetItem *itemPunkte = new QTableWidgetItem( QString::number(question.points(),10) );
//        QTableWidgetItem *itemMaxPunkte = new QTableWidgetItem( QString::number(question.maxPoints(),10) );
//        QTableWidgetItem *itemKennung = new QTableWidgetItem( question.identifier() );
//        ui->fragenTableWidget->setItem(row,0, itemNr);
//        ui->fragenTableWidget->setItem(row,1, itemFrage);
//        ui->fragenTableWidget->setItem(row,2, itemPunkte);
//        ui->fragenTableWidget->setItem(row,3, itemMaxPunkte);
//        ui->fragenTableWidget->setItem(row,4, itemKennung);

//        itemNr->setFlags(Qt::ItemIsEnabled);
//        itemFrage->setFlags(Qt::ItemIsEnabled);
//        itemMaxPunkte->setFlags(Qt::ItemIsEnabled);
//        itemKennung->setFlags(Qt::ItemIsEnabled);
//        itemPunkte->setTextColor(Qt::blue);


//        row++;
//    }

//    ui->fragenTableWidget->resizeColumnToContents(0);
//    ui->fragenTableWidget->resizeColumnToContents(1);
//    ui->fragenTableWidget->resizeColumnToContents(2);
//    ui->fragenTableWidget->resizeColumnToContents(3);
//    ui->fragenTableWidget->resizeColumnToContents(4);

//    connect(ui->fragenTableWidget, &QTableWidget::cellChanged, this, &FormEvaluation::questionTableCellChanged);

//}

//void FormEvaluation::updateSkillBox(const ClassLehrling &azu)
//{
//    ui->skillListBox->clear();
//    ui->projektListBox->clear();
//    ui->fragenTableWidget->clear();
//    ui->projektNameEdit->clear();
//    ui->percentBox->setValue(0);


//    if(azu.getSkillMap().isEmpty()){
//        setTextColor(ui->azubiListBox, Qt::red);
//        selectedSkill = ClassSkills();
//        ui->countSkillBox->setValue(0);
//    }else{
//        setTextColor(ui->azubiListBox, Qt::blue);
//        ui->skillListBox->addItems(selectedApprentice.getSkillMap().keys());
//        ui->countSkillBox->setValue(selectedApprentice.getSkillMap().values().size());
//    }

//    ui->nrBox->setValue(azu.nr());
//    ui->azuNameEdit->setText(azu.firstname()+"."+azu.surname());
//}

void FormEvaluation::closeButtonClicked()
{
    if(dirty){
        int result = QMessageBox::information(this,"Auswertung schließen", "Die Auswertungen haben sich geändert.\n"
                                 "Wenn sie jetzt schließen gehen die eingegebenen Daten verloren!",
                                  QMessageBox::Save | QMessageBox::Cancel);
         if(result == QMessageBox::Save)
             saveButtonClicked();
    }

    emit formEvaluationClosed();
    close();
}

void FormEvaluation::saveButtonClicked()
{
    emit saveApprenticeMap(m_apprenticeMap);
    ui->saveButton->setEnabled(false);
    dirty = false;

    if(ui->evaluatedCheckBox->isChecked())
        ui->factorBox->setEnabled(false);

}

/// !brief Show a list of results
void FormEvaluation::listButtonClicked()
{
    QList<ClassLehrling> aList;

    for(int i = 0; i < ui->azubiListBox->count(); i++)
          aList << m_apprenticeMap.value( ui->azubiListBox->itemText(i) );

    apprenticeTableList->setapprenticeList(aList);
    apprenticeTableList->setCaption(ui->azubiSortBox->currentText());
    apprenticeTableList->show();
}

void FormEvaluation::printButtonClicked(const QMap<int, QVariant> &map)
{
    emit printResultList(map);
}

/// !brief Includes all apprentice keys sorted by years
void FormEvaluation::azubiSortBoxChanged(const QString &text)
{
    if(text.isEmpty())
        return;

    QStringList list;
    if(text.at(0).isDigit()){
        QString s = text.at(0);
        int year = s.toInt();
        list = getApprenticeKeyList( year );
    }else
        list = getApprenticeKeyList(5);

    if(!list.isEmpty()){
        ui->azubiListBox->clear();

        // Sort by skill nr
        QMap<int, QString> sortedMap;

        for (int i = 0; i < list.size(); i++) {
            QStringList split = list.at(i).split(".");
            if(split.size() != 3)
                QMessageBox::warning(this, "Fehler",  "In der Namensgebung eine Auszubildenden:"+ list.at(i) +" wurde ein Fehler bemerkt.\n"
                                                      "Bitte korrigieren sie den Fehler!");
            else
            {
                QString snr = split.last();
                int nr = snr.toInt();
                sortedMap.insert(nr, list.at(i));
            }

        }


        ui->azubiListBox->addItems(  sortedMap.values() );  // is sorted by name
        //ui->azubiListBox->addItems(  list );  // is sorted by name
        ui->countAzubiBox->setValue( list.size()  );
    }
}

/// !brief Includes all apprentice by name
void FormEvaluation::azubiListBoxChanged(const QString &text)
{
    if(text.isEmpty())
        return;

    ui->skillListBox->clear();
    selectedApprentice = getApprenticeMap().value(text);

    ui->nrBox->setValue( selectedApprentice.nr() );
    ui->azuNameEdit->setText( selectedApprentice.firstname() + " " + selectedApprentice.surname());

    ui->countSkillBox->setValue( selectedApprentice.getSkillMap().size() );

    // Sorted by name (keys)
    ui->skillListBox->addItems( selectedApprentice.getSkillMap().keys() );


    updateResultTreeWidget(selectedApprentice);

    if(selectedApprentice.getSkillTotalPercent() != 100 || selectedApprentice.getSkillMap().isEmpty())
    {
        QString message;
        QPixmap pix;
        if(selectedApprentice.getSkillMap().isEmpty())
        {
            message = "Eine Auswertung ist nicht möglich, da keine Prüfung zugeordnet wurde.";
            pix = QPixmap(":/images/Attention.svg");
        }
        else
        {
            message = "Achtung, die Gewichtung der gesamten Prüfung muss insgesamt 100% betragen. "
                      "Das ist hier nicht der Fall. Korrigieren sie die Wertung bzw. Faktor dieser Prüfung!";
            pix = QPixmap(":/images/AttentionPercent.svg");
        }

        sendMessage(message, pix, true);


    }
    else
    {
        sendMessage(QString(""), QPixmap(), false);
    }
}

/// !brief Includes the skills from selected apprentice
void FormEvaluation::azubiSkillBoxChanged(const QString &text)
{

    selectedSkill = selectedApprentice.getSkillMap().value(text); //  selectedSkillMap.value(text);
    //selectedProjectMap = selectedSkill.getProjektMap();

    ui->projektListBox->clear();
    ui->projektListBox->addItems( selectedSkill.getProjektMap().keys());
    ui->countProjektBox->setValue( selectedSkill.getProjektMap().size() );
}

/// !brief Update the values of the selected apprentice
void FormEvaluation::azubiProjectBoxChanged(const QString &text)
{
    selectedProjekt = selectedSkill.getProjektMap().value(text);

    ui->projectLabel->setText(selectedProjekt.name());

    setTextColor(ui->percentBox, selectedProjekt.percent());

    ui->percentBox->setValue( selectedProjekt.percent() );
    ui->evaluatedCheckBox->setChecked( selectedProjekt.getEvaluated() );
    ui->factorBox->setValue( selectedProjekt.getFactor() );

    setupQuestionTable(selectedProjekt);
}

void FormEvaluation::evaluatedCheckBoxChanged(int status)
{
    if(status == Qt::Checked && selectedProjekt.getEvaluated() != true){
        selectedProjekt.setEvaluated(true);
        ui->factorBox->setEnabled(false);
        storeValues();
    }

    if(status == Qt::Unchecked  && selectedProjekt.getEvaluated() == true){
        selectedProjekt.setEvaluated(false);
        ui->factorBox->setEnabled(true);
        storeValues();
    }

    if(status == Qt::Checked  && selectedProjekt.getEvaluated() == true){
        ui->factorBox->setEnabled(false);
    }
}

void FormEvaluation::questionTableCellChanged(int row, int column)
{
    if(column != 1)
        return;

    if(ui->evaluatedCheckBox->isChecked()){
        QMessageBox::information(this,"Auswertung", "Das Projekte wurde bereits ausgewertet.\n"
                                     "Für Änderungen deselektieren sie bitte Ausgewertet!");

        // Get old value and put it back to cell
        int v = selectedProjekt.questionMap().value( row ).points();
        ui->questionTableWidget->item(row, 1)->setText( QString::number(v,10));

        return;
    }

    QString val = ui->questionTableWidget->item(row, column)->text();

    if(!isDigit(val)){
        QMessageBox::information(this, tr("Eingabe"), tr("Bitte nur Zahlen eingeben!"));
        ui->questionTableWidget->item(row, column)->setText("0");
        return;
    }

    int point = val.toInt();
    if(point > ui->questionTableWidget->item(row, 2)->text().toInt()){
        QMessageBox::information(this, tr("Eingabe"), tr("Zahlenwert zu hoch!"));
        ui->questionTableWidget->item(row, column)->setText("0");
        return;
    }

    selectedProjekt.setQuestionPoints(row, point);
    double proPercent = projectPercent(selectedProjekt);
    if(proPercent >= 50.0)
        setTextColor(ui->percentBox, Qt::darkGreen);
    else
        setTextColor(ui->percentBox, Qt::red);

    ui->percentBox->setValue( proPercent );
    selectedProjekt.setPercent(proPercent);
    selectedProjekt.setPoints( projectPoints(selectedProjekt) );
    storeValues();

}

/// !brief When clicking on item then
/// set the current project or skill
void FormEvaluation::resultTableItemClicked(QTreeWidgetItem *item, int )
{
    QString key = item->text(0);

    // User clicked on top item -> skill was choosed
    if(selectedApprentice.getSkillMap().keys().contains(key)){
        ui->skillListBox->setCurrentText(key);
        return;
    }

    // User clicked on child item -> project was choosed
    if(selectedSkill.getProjektMap().keys().contains(key)){
        ui->projektListBox->setCurrentText(key);
    }
}

/// !brief Sort apprentice by years of apprenticeship
void FormEvaluation::setupApprYearBox()
{
    if(m_apprenticeMap.isEmpty())
        return;

    QStringList keyList;
    foreach (ClassLehrling appr, getApprenticeMap().values())
    {
        int teachYear = appr.apprenticeshipDate().daysTo(QDate::currentDate()) / 365;
        teachYear++;
        if(teachYear > 5)
            teachYear = 5;
        QString name = QString::number(teachYear,10)+".Lehrjahr";
        if(!keyList.contains(name))
            keyList << name;
    }

    ui->azubiSortBox->clear();
    ui->azubiSortBox->addItems(keyList);
}

QStringList FormEvaluation::getApprenticeKeyList(int year)
{
    QStringList list;

    QMapIterator<QString, ClassLehrling> it(m_apprenticeMap);
    while (it.hasNext()) {
        it.next();
        int educationYear = it.value().apprenticeshipDate().daysTo(QDate::currentDate()) / 365;
        educationYear++;
        if(educationYear > 5)
            educationYear = 5;

        if(educationYear == year)
            list << it.key();
    }

    return list;

}

/// !brief Update the questions from the selected project
void FormEvaluation::setupQuestionTable(const ClassProjekt &pro)
{
    disconnect(ui->questionTableWidget, &QTableWidget::cellChanged, this, &FormEvaluation::questionTableCellChanged);

    ui->questionTableWidget->clear();
    ui->questionTableWidget->setColumnCount(4);
    ui->questionTableWidget->setRowCount(pro.questionMap().size());

    QStringList labels;
    labels << "Frage" << "Punkte" << "Max. Punkte" << "Kennung" ;
    ui->questionTableWidget->setHorizontalHeaderLabels(labels);

    QMap<int, ClassFrage> qMap = pro.questionMap();
    int row = 0;

    QMapIterator<int, ClassFrage> it(qMap);
    while (it.hasNext()) {
        it.next();

        ClassFrage question = it.value();
        QTableWidgetItem *itemQuestion = new QTableWidgetItem( question.question() );
        QTableWidgetItem *itemPoints = new QTableWidgetItem( QString::number(question.points(),10) );
        QTableWidgetItem *itemMaxPoints = new QTableWidgetItem( QString::number(question.maxPoints(),10) );
        QTableWidgetItem *itemIdent = new QTableWidgetItem( question.identifier() );

        ui->questionTableWidget->setItem(row,0, itemQuestion);
        ui->questionTableWidget->setItem(row,1, itemPoints);
        ui->questionTableWidget->setItem(row,2, itemMaxPoints);
        ui->questionTableWidget->setItem(row,3, itemIdent);

        itemQuestion->setFlags(Qt::ItemIsEnabled);
        itemMaxPoints->setFlags(Qt::ItemIsEnabled);
        itemIdent->setFlags(Qt::ItemIsEnabled);
        itemPoints->setTextColor(Qt::blue);

        itemPoints->setToolTip(tr("Geben sie hier die erreichte Punktzahl ein."));

        row++;

    }

    ui->questionTableWidget->resizeColumnsToContents();
    connect(ui->questionTableWidget, &QTableWidget::cellChanged, this, &FormEvaluation::questionTableCellChanged);
}

double FormEvaluation::apprenticePercent(const ClassLehrling &)
{
    double percent = 0.0;
    foreach (ClassSkills skill, selectedApprentice.getSkillMap().values()) {
        if(selectedApprentice.getSkillMap().size() > 1)
            percent += skillPercent(skill) * (skill.getWert()/100.0);
        else
            percent += skillPercent(skill);

    }
    return percent;
}

double FormEvaluation::skillPercent( ClassSkills skill)
{
    double percent = 0.0;

    if(skill.getEvaluationIndex() == 0)
    {
        foreach (ClassProjekt p, skill.getProjektMap().values())
            percent += projectPercent(p) * p.getFactor();

    }

    if(skill.getEvaluationIndex() == 1)
    {
        QStringList list = skill.getIdentifierList();
        for(int i = 0; i < list.size(); i++)
            percent += skill.getIdentPercent(list.at(i)) * skill.getIdentFactor(list.at(i));

        foreach (ClassProjekt pro, skill.getProjektMap().values())
        {
            if(pro.identifierList().isEmpty())
            {
                percent += projectPercent(pro) * pro.getFactor();
            }
        }
    }

    return percent;
}

double FormEvaluation::projectPercent(const ClassProjekt &pro)
{
    double percent = 0.0;
    int points = 0;
    foreach (ClassFrage q, pro.questionMap().values()) {
        points += q.points();
    }

    return percent = points * 100.0 / pro.maxPoints();
}

int FormEvaluation::projectPoints(const ClassProjekt &pro)
{
    int points = 0;
    foreach (ClassFrage q, pro.questionMap().values()) {
        points += q.points();
    }
    return points;
}

double FormEvaluation::totalProjectPercent(const ClassSkills &skill)
{
    qreal percent = 0.0;
    foreach (ClassProjekt pro, skill.getProjektMap().values()) {
        percent += pro.getFactor() * 100.0;
    }
    return percent;
}

void FormEvaluation::storeValues()
{
    selectedSkill.insertProjekt(selectedProjekt);
    selectedApprentice.insertSkill(selectedSkill);
    m_apprenticeMap.insert(selectedApprentice.getKey(), selectedApprentice);
    updateResultTreeWidget(selectedApprentice);

    dirty = true;
    ui->saveButton->setEnabled(true);
}

void FormEvaluation::updateResultTreeWidget(const ClassLehrling &appr)
{
    ui->resultTreeWidget->clear();

    QStringList headers;
    headers << "Titel" << "Ergebnis in %" << "Wert/Faktor" << "Auswertung nach";
    ui->resultTreeWidget->setHeaderLabels(headers);

    QFont topItemFont;
    double total = 0.0;

    // Calculate results
    QMapIterator<QString, ClassSkills> it(appr.getSkillMap());
    while (it.hasNext())
    {
        it.next();

        ClassSkills skill = it.value();

        // If skill evaluation method is project
        double tpp = totalProjectPercent(skill);
        QTreeWidgetItem *topItem = new QTreeWidgetItem(QStringList() << it.key());
        ui->resultTreeWidget->addTopLevelItem(topItem);
        if(tpp < 99.00 || tpp > 100.0)
        {
            topItem->setForeground(0, QBrush(QColor(255,0,0)));
            topItem->setText(0, topItem->text(0).append("  !"));
            topItem->setDisabled(true);
            topItem->setToolTip(0,tr("Die Faktoren der Projekte müssen korrigiert werden!"));
            QPixmap pix(":/images/AttentionPercent.svg");
            QString message = "Prüfung: " + it.key() + " ist fehlerhaft!"
                    "\nDie Summe der Faktoren von den Projekten müssen korrigiert werden!";
            sendMessage( message, pix , true);
        }

        ui->resultTreeWidget->expandItem(topItem);

        // Shows the percent of skill, calculated by project with factor
        topItem->setText(1, QString::number( skillPercent(skill),'g',2)+"%");
        setItemColor(topItem,  skillPercent(skill) );

        double skillFactor = skill.getWert() / 100.0;

        if(appr.getSkillMap().size() == 1 && skillFactor < 1.0)
        {
            skillFactor = 1.0;
            QString s = "100% (" + QString::number( skill.getWert() ,'g',3)+"%)";
            topItem->setText(2, s);
        }
        else
            topItem->setText(2, QString::number( skill.getWert() ,'g',3)+"%");

        topItem->setText(3, skill.getEvaluationText( skill.getEvaluationIndex() ));

        // Set top item font
        topItemFont = topItem->font(0);
        topItemFont.setBold(true);
        topItem->setFont(0,topItemFont);
        topItem->setFont(1,topItemFont);
        topItem->setFont(2,topItemFont);

        if(skill.getEvaluationIndex() == 1 && !skill.getIdentifierList().isEmpty())
        {
            QStringList iList = skill.getIdentifierList();
            for(int i = 0; i < iList.size() ; i++)
            {
                QTreeWidgetItem *childItem = new QTreeWidgetItem(QStringList() << iList.at(i));
                topItem->addChild(childItem);

                childItem->setText(1, QString::number( skill.getIdentPercent( iList.at(i) ) ,'g', 3) + "%" );
                childItem->setText(2, QString::number( skill.getIdentFactor( iList.at(i)) ,'g', 4));
                setItemColor(childItem,  skill.getIdentPercent( iList.at(i) ) );

            }

            foreach (ClassProjekt pro, skill.getProjektMap().values())
            {
                if(pro.identifierList().isEmpty() && pro.isValid())
                {
                    QTreeWidgetItem *childItem = new QTreeWidgetItem(QStringList() << pro.getKey());
                    topItem->addChild(childItem);
                    childItem->setText(1, QString::number(pro.percent(), 'g', 3)+" %");
                    childItem->setText(2, QString::number(pro.getFactor(), 'g', 4));
                    setItemColor(childItem, pro.percent());

                }
            }
        }

        // Add projects as child item
        foreach (ClassProjekt pro, skill.getProjektMap().values())
        {

            if(skill.getEvaluationIndex() == 0 && pro.isValid())
            {

                QTreeWidgetItem *childItem = new QTreeWidgetItem(QStringList() << pro.getKey());
                topItem->addChild(childItem);
                childItem->setText(1, QString::number(pro.percent(), 'g', 3)+" %");
                childItem->setText(2, QString::number(pro.getFactor(), 'g', 3));
                setItemColor(childItem, pro.percent());
            }

            ui->resultTreeWidget->resizeColumnToContents(0);
        }

        total += skillPercent(skill) * skillFactor;
    }


    ui->resultTreeWidget->resizeColumnToContents(0);

    ui->totalPercentBox->setValue( total );
    setTextColor(ui->totalPercentBox,  total );
    ui->gradeBox->setValue( FSHKWindow::grade(total) );


    // Update the apprenticeTableList
    if(!apprenticeTableList->isHidden())
    {
        QList<ClassLehrling> aList;

        for(int i = 0; i < ui->azubiListBox->count(); i++)
              aList << m_apprenticeMap.value( ui->azubiListBox->itemText(i) );

        apprenticeTableList->setapprenticeList(aList);
    }

}


bool FormEvaluation::isDigit(const QString &text)
{
    bool status = true;
    foreach (QChar c, text) {
        if(!c.isDigit())
            status = false;
    }
    return status;
}

void FormEvaluation::setTextColor(QWidget *widget, double percent)
{
    QPalette pal;
    QColor c;

    pal = widget->palette();
    if(percent < 49.5)
        c = Qt::red;
    else
        c = Qt::darkGreen;

    pal.setColor(QPalette::WindowText, c);
    pal.setColor(QPalette::Text, c);
    widget->setPalette(pal);
}

void FormEvaluation::setItemColor(QTreeWidgetItem *item, double percent)
{
    if(percent < 49.5)
        item->setTextColor(1, Qt::red);
    else
        item->setTextColor(1, Qt::darkGreen);
}

void FormEvaluation::sendMessage(const QString &text, const QPixmap &pixmap, bool show)
{
    ui->labelIcon->setPixmap( pixmap );
    ui->labelMessage->setText( text );
    if(show)
        ui->frameAttention->show();
    else
        ui->frameAttention->hide();
}

//void FormEvaluation::saveButtonClicked()
//{
//    if(!isSkillFactorValid()){
//        QMessageBox::information(this,"Faktor", "Die Gewichtung der Projekte muss insgesamt 1.0 betragen.\n"
//                                         "Bitte korrigieren sie die Faktoren der Projekte!");
//        return;
//    }

//    emit saveApprenticeMap(m_apprenticeMap);
//    ui->saveButton->setEnabled(false);
//    dirty = false;
//}

//void FormEvaluation::recoverButtonClicked()
//{
//    emit recoverAll();
//}


//void FormEvaluation::setupResultWidget(const ClassLehrling &azu)
//{
//    ui->resultTreeWidget->clear();

//    QMap<QString, ClassSkills> sMap;
//    sMap = azu.getSkillMap();

//    ui->resultTreeWidget->setColumnCount(4);

//    QStringList headers;
//    headers << "Beschreibung" << "Ergebnis in %" << "Wert/Faktor" << "Auswertung nach";
//    ui->resultTreeWidget->setHeaderLabels(headers);

//    QMapIterator<QString, ClassSkills> it(sMap);
//    while (it.hasNext()) {
//        it.next();

//        double skillPercent = 0.0;

//        ClassSkills skill = it.value();
//        bool projectNode = false;

//        if( skill.getEvaluationIndex() == 0 )
//            projectNode = true;

//        QTreeWidgetItem *topItem = new QTreeWidgetItem(QStringList() << it.key());
//        ui->resultTreeWidget->addTopLevelItem(topItem);
//        ui->resultTreeWidget->expandItem(topItem);

//        QFont font = topItem->font(0);
//        font.setBold(true);
//        topItem->setFont(0,font);
//        topItem->setFont(1,font);

//        // Setup child item from skill
//        QMap<QString, ClassProjekt> pMap;
//        pMap = skill.getProjektMap();

//        QMapIterator<QString, ClassProjekt> ip(pMap);
//        while (ip.hasNext()) {
//             ip.next();
//             ClassProjekt pro = ip.value();

//             QTreeWidgetItem *childItem = new QTreeWidgetItem(QStringList() << ip.key());
//             topItem->addChild(childItem);

//             // if project calculate by identifier
//             if(!pro.identifierList().isEmpty() && skill.getEvaluationIndex() == 1){
//                QStringList list = pro.identifierList();
//                foreach (QString ids, list) {
//                    QTreeWidgetItem *cc = new QTreeWidgetItem(QStringList() << ids);
//                    childItem->addChild(cc);

//                    double identPercent =  getResultIdentifier(  getQuestions(pro, ids) );
//                    cc->setText(1, QString::number(identPercent, 'g', 3)+ "%" );
//                }
//             }

//             // Set percent each project
//             if(pro.percent() < 50.0){
//                 childItem->setTextColor(0, Qt::red);
//                 childItem->setTextColor(1, Qt::red);
//             }else{
//                 childItem->setTextColor(0, Qt::darkGreen);
//                 childItem->setTextColor(1, Qt::darkGreen);
//             }

//             childItem->setText(1, QString::number(pro.percent(), 'g', 3)+" %");
//             childItem->setText(2, QString::number(pro.getFactor(), 'g', 2));

//             // calculate the skill percent
//             if(projectNode)
//             {
//                 double proPercent = pro.percent() * pro.getFactor();
//                 skillPercent = skillPercent + proPercent;
//             }
//        }

//        // Set skill results
//        topItem->setText(1, QString::number( skillPercent,'g',3)+"%");

//        // If only one skill show info
//        if(sMap.size() == 1){
//            QString text = "100%("+QString::number( skill.getWert(),10)+"%)";
//            topItem->setText(2, text);
//        }else
//            topItem->setText(2, QString::number( skill.getWert(),10)+"%");

//        // Choose color
//        if(skillPercent < 50.0)
//            topItem->setTextColor(1,Qt::red);
//        else
//            topItem->setTextColor(1,Qt::darkGreen);

//        // Set info for evaluation criteria for skill
//        QString criteriaText = skill.getEvaluationText( skill.getEvaluationIndex() );
//        topItem->setText(3, criteriaText);



//    }

//    ui->resultTreeWidget->resizeColumnToContents(0);


//    double totalPercent = 0.0;
//    int index = 0;
//    QMapIterator<QString, ClassSkills> it(sMap);
//    while (it.hasNext()) {
//        it.next();

//        ClassSkills skill = it.value();

//        QTreeWidgetItem *topItem = new QTreeWidgetItem(QStringList() << it.key());

//        ui->resultTreeWidget->addTopLevelItem(topItem);
//        ui->resultTreeWidget->expandItem(topItem);

//        double percent = skill.points() * 100.0 / skill.maxPoints();
//        topItem->setText(1, QString::number(percent, 'g',3));

//        QString wert = QString::number( skill.getWert(), 10 )+"%";
//        if(sMap.size() == 1)
//            wert.append("(100%)");

//        topItem->setText(2, wert);


//        QFont font = topItem->font(0);
//        font.setBold(true);
//        topItem->setFont(0,font);
//        topItem->setFont(1,font);


//        double tp = 0.0;
//        if(sMap.size() == 1)
//            tp = percent * (100/100.0);
//        else
//            tp = percent * (skill.getWert()/100.0);


//        totalPercent = totalPercent + tp;

//        // Set all projects as child items
//        QMap<QString, ClassProjekt> pMap;
//        pMap = skill.getProjektMap();

//        QMapIterator<QString, ClassProjekt> ip(pMap);
//        while (ip.hasNext()) {
//            ip.next();
//            ClassProjekt pro = ip.value();

//            QTreeWidgetItem *childItem = new QTreeWidgetItem(QStringList() << ip.key());
//            topItem->addChild(childItem);

//            QString pers = QString::number(pro.percent(),'g' , 3);
//            QString fact = QString::number(pro.getFactor(),'g' , 3);
//            childItem->setText(1, pers);
//            childItem->setText(2, fact);

//            if(pro.percent() < 50.0){
//                childItem->setTextColor(0, Qt::red);
//                childItem->setTextColor(1, Qt::red);
//            }else{

//                childItem->setTextColor(0, Qt::black);
//                childItem->setTextColor(1, Qt::darkGreen);
//            }
//        }

//        index++;

//    }

//    // Total result from exame
//    QTreeWidgetItem *topItem = new QTreeWidgetItem(QStringList() << "Prüfungsergebnis:");
//    ui->resultTreeWidget->addTopLevelItem(topItem);
//    ui->resultTreeWidget->expandItem(topItem);
//    QFont font = topItem->font(0);
//    font.setBold(true);
//    topItem->setFont(0,font);
//    topItem->setFont(1,font);

//    if(totalPercent < 50.0)
//        topItem->setTextColor(1, Qt::red);
//    else
//        topItem->setTextColor(1, Qt::darkGreen);



//    topItem->setText(1, QString::number(totalPercent, 'g',3));


//    ui->resultTreeWidget->resizeColumnToContents(0);
//    ui->resultTreeWidget->resizeColumnToContents(1);

//    setupIdentifier(azu);
//}

//void FormEvaluation::setupIdentifier(const ClassLehrling &azu)
//{

//    QMultiMap<QString, ClassFrage> identifierMap;

//    QMap<QString, ClassSkills> sMap;
//    sMap = azu.getSkillMap();

//    QMapIterator<QString, ClassSkills> it(sMap);
//    while (it.hasNext()) {
//        it.next();

//        ClassSkills skill = it.value();

//        // Set all projects as child items
//        QMap<QString, ClassProjekt> pMap;
//        pMap = skill.getProjektMap();

//        QMapIterator<QString, ClassProjekt> ip(pMap);
//        while (ip.hasNext()) {
//            ip.next();
//            ClassProjekt pro = ip.value();

//            QMap<int, ClassFrage> iMap;
//            iMap = pro.questionMap();
//            QMapIterator<int, ClassFrage> iq(iMap);
//            while (iq.hasNext()) {
//                iq.next();
//                ClassFrage frg = iq.value();
//                if(!frg.identifier().isEmpty())
//                    identifierMap.insert(frg.identifier(),frg);

//            }

//        }
//    }

//    if(identifierMap.isEmpty())
//        return;

//    QStringList keys = identifierMap.keys();
//    QStringList headers;

//    foreach (QString k , keys) {

//        // Create top item
//        if(!headers.contains(k)){
//            headers << k;
//            QTreeWidgetItem *topItem = new QTreeWidgetItem(QStringList() << k);
//            ui->resultTreeWidget->addTopLevelItem(topItem);

//            double percent = getResultIdentifier(identifierMap.values(k));
//            topItem->setText(1, QString::number(percent, 'g', 3));
//        }
//    }
//}

//double FormEvaluation::getResultIdentifier(const QList<ClassFrage> questList)
//{
//    double percent = 0.0;
//    int points = 0;
//    int maxpoints = 0;
//    foreach (ClassFrage frg, questList) {
//        maxpoints = maxpoints + frg.maxPoints();
//        points = points + frg.points();
//    }

//    percent = points * 100.0 / maxpoints;

//    return percent;
//}

//bool FormEvaluation::isSkillFactorValid()
//{
//    QMap<int, QVariant> rMap;
//    int index = 0;
//    foreach (ClassSkills sk, selectedApprentice.getSkillMap().values())
//    {
//        if(getSkillFactor(sk) != 1.0)
//            rMap.insert(index, false);
//        else
//            rMap.insert(index, true);

//        index++;
//    }

//    if(rMap.keys(true).size() == selectedApprentice.getSkillMap().size())
//        return true;
//    else
//        return false;
//}

//double FormEvaluation::getSkillFactor(const ClassSkills &skill)
//{
//    double fac = 0.0;

//    foreach (ClassProjekt pro, skill.getProjektMap().values()) {
//        fac += pro.getFactor();
//    }

//    return fac;
//}

//QStringList FormEvaluation::questionsIdentifierList(const ClassLehrling &azu)
//{
//    QStringList identList;
//    QMap<QString, ClassSkills> sMap;
//    sMap = azu.getSkillMap();
//    QMapIterator<QString, ClassSkills> it(sMap);
//    while (it.hasNext()) {
//        it.next();
//        ClassSkills skill = it.value();
//        QMap<QString, ClassProjekt> pMap;
//        pMap = skill.getProjektMap();
//        QMapIterator<QString, ClassProjekt> ip(pMap);
//        while (ip.hasNext()) {
//            ip.next();
//            ClassProjekt pro = ip.value();
//            foreach (ClassFrage frg, pro.questionMap().values()) {
//                if(!identList.contains(frg.identifier()))
//                    identList << frg.identifier();
//            }
//        }
//    }
//    return identList;
//}

//QList<ClassFrage> FormEvaluation::getQuestions(const ClassProjekt &pro, const QString &key)
//{
//    QList<ClassFrage> qList;
//    QMapIterator<int, ClassFrage> it(pro.questionMap());
//    while (it.hasNext()) {
//        it.next();
//        if(it.value().identifier() == key)
//            qList << it.value();

//    }
//    return qList;
//}

//double FormEvaluation::getProjectPercent(const ClassProjekt &pro)
//{
//    double percent = 0.0;
//    int maxPoint = getProjectMaxPoint(pro);
//    int points = getProjectPoints(pro);

//    percent = points * 100.0 / maxPoint;

//    return percent;
//}

//int FormEvaluation::getProjectMaxPoint(const ClassProjekt &pro)
//{
//    int mp = 0;
//    foreach (ClassFrage frg, pro.questionMap().values()) {
//        mp = mp + frg.maxPoints();
//    }
//    return mp;
//}

//int FormEvaluation::getProjectPoints(const ClassProjekt &pro)
//{
//    int p = 0;
//    foreach (ClassFrage frg, pro.questionMap().values()) {
//        p = p + frg.points();
//    }
//    return p;
//}

//bool FormEvaluation::isSkillKey(const QString &text)
//{
//    return selectedApprentice.getSkillMap().keys().contains(text);
//}



























































