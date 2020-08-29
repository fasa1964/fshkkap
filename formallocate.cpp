#include "formallocate.h"
#include "ui_formallocate.h"

#include <QDebug>

FormAllocate::FormAllocate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAllocate)
{
    ui->setupUi(this);

    selectedSkill = ClassSkills();

    connect(ui->closeButton, &QPushButton::clicked, this, &FormAllocate::closeButtonClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &FormAllocate::saveButtonClicked);
    connect(ui->importToolButton, &QToolButton::clicked, this, &FormAllocate::importButtonClicked);
    connect(ui->skillsListWidget, &QListWidget::itemClicked, this, &FormAllocate::skillTableItemClicked);
}

FormAllocate::~FormAllocate()
{
    delete ui;
}

void FormAllocate::closeButtonClicked()
{
    emit formAllocateClosed();
    close();
}

void FormAllocate::saveButtonClicked()
{
    emit saveApprenticeMap(apprenticeMap());
    ui->saveButton->setEnabled(false);
}

void FormAllocate::importButtonClicked()
{
    QMap<QString, ClassLehrling> selectedAppMap;

    if(ui->tabWidget->currentIndex() == 0)
        selectedAppMap = getSelectedApprentice(ui->apprenticeTableWidget);

    if(ui->tabWidget->currentIndex() == 1)
        selectedAppMap = getSelectedApprentice(ui->app1Table);

    if(ui->tabWidget->currentIndex() == 2)
        selectedAppMap = getSelectedApprentice(ui->app2Table);

    if(ui->tabWidget->currentIndex() == 3)
        selectedAppMap = getSelectedApprentice(ui->app3Table);

    if(ui->tabWidget->currentIndex() == 4)
        selectedAppMap = getSelectedApprentice(ui->app4Table);

    if(ui->tabWidget->currentIndex() == 5)
        selectedAppMap = getSelectedApprentice(ui->app5Table);

    if(!selectedAppMap.isEmpty()){
        foreach (ClassLehrling app, selectedAppMap.values()) {
            QMap<QString, ClassSkills> sMap = app.getSkillMap();
            sMap.insert(selectedSkill.getKey(), selectedSkill);
            app.setSkillMap(sMap);
            m_apprenticeMap.insert(app.getKey(), app);
        }

        setApprenticeMap(m_apprenticeMap);
        ui->saveButton->setEnabled(true);
    }
}

void FormAllocate::skillTableItemClicked(QListWidgetItem *item)
{

    if(skillMap().isEmpty())
        return;

    ui->selectedSkillEdit->setText( item->text() );
    selectedSkill = skillMap().value( item->text() );

    if(!selectedSkill.name().isEmpty())
        ui->importToolButton->setEnabled(true);
    else
        ui->importToolButton->setEnabled(false);

}

void FormAllocate::setupSkillTable(const QMap<QString, ClassSkills> &sMap)
{
    ui->skillsListWidget->clear();

    QMapIterator<QString, ClassSkills> it(sMap);
    while (it.hasNext()) {
        it.next();
        ClassSkills skill = it.value();
        ui->skillsListWidget->addItem( skill.getKey() );
    }

}

QMap<QString, ClassSkills> FormAllocate::skillMap() const
{
    return m_skillMap;
}

void FormAllocate::setSkillMap(const QMap<QString, ClassSkills> &skillMap)
{
    m_skillMap = skillMap;
//    ui->sortSkillBox->clear();
//    ui->sortSkillBox->addItems( getIdientifierList() );

    setupSkillTable(skillMap);
}

QMap<QString, ClassLehrling> FormAllocate::apprenticeMap() const
{
    return m_apprenticeMap;
}

void FormAllocate::setApprenticeMap(const QMap<QString, ClassLehrling> &apprenticeMap)
{
    m_apprenticeMap = apprenticeMap;
    sortApprenticeTable();
}

/// !brief Sort the table by year of training
void FormAllocate::sortApprenticeTable()
{

    updateApprenticeTable(ui->apprenticeTableWidget, Qt::Unchecked, apprenticeMap());

    for(int i = 1; i < 6; i++)
    {
        QMap<QString, ClassLehrling> sortMap;
        sortMap = getApprenticeMap(i);
        ui->tabWidget->setTabEnabled(i,false);

        if(i == 1 && !sortMap.isEmpty()){
            ui->tabWidget->setTabEnabled(i,true);
            updateApprenticeTable(ui->app1Table, Qt::Checked, sortMap);
        }
        if(i == 2 && !sortMap.isEmpty()){
            ui->tabWidget->setTabEnabled(i,true);
            updateApprenticeTable(ui->app2Table,  Qt::Checked, sortMap);
        }
        if(i == 3 && !sortMap.isEmpty()){
            ui->tabWidget->setTabEnabled(i,true);
            updateApprenticeTable(ui->app3Table,  Qt::Checked, sortMap);
        }

        if(i == 4 && !sortMap.isEmpty()){
            ui->tabWidget->setTabEnabled(i,true);
            updateApprenticeTable(ui->app4Table,  Qt::Checked, sortMap);
        }
        if(i == 5 && !sortMap.isEmpty()){
            ui->tabWidget->setTabEnabled(i,true);
            updateApprenticeTable(ui->app5Table, Qt::Checked, sortMap);
        }
    }
}

/// !brief Returns a map sorted by year of training
QMap<QString, ClassLehrling> FormAllocate::getApprenticeMap(int year)
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

void FormAllocate::updateApprenticeTable(QTableWidget *widget, Qt::CheckState state, const QMap<QString, ClassLehrling> &aMap)
{
    widget->clear();
    widget->setRowCount(aMap.size());
    widget->setColumnCount(6);

    QStringList labels;
    labels << "Nr." << "Name" << "Klasse" << "Betrieb" << "AP's" << "Marker";
    widget->setHorizontalHeaderLabels(labels);

    int row = 0;
    QMapIterator<QString, ClassLehrling> it(aMap);
    while (it.hasNext()) {
        it.next();
        ClassLehrling appr = it.value();

        QFont tableFont = ui->apprenticeTableWidget->font();
        QString ap = "";
        bool skillIsEvaluated = false;

        if(it.value().getSkillMap().size() > 0){
            foreach(ClassSkills skill, it.value().getSkillMap().values()){

                ap.append(skill.getKey());
                if(skill.isEvaluated()){
                    ap.append("!");
                    skillIsEvaluated = true;
                }
                ap.append("\n");
           }
        }

        QTableWidgetItem *itemNr = new QTableWidgetItem( QString::number(appr.nr(),10));
        QTableWidgetItem *itemName = new QTableWidgetItem( it.key() );
        QTableWidgetItem *itemKlasse = new QTableWidgetItem( appr.educationClass() );
        QTableWidgetItem *itemBetrieb = new QTableWidgetItem( appr.company() );
        QTableWidgetItem *itemAP = new QTableWidgetItem(ap);
        QTableWidgetItem *itemMarker = new QTableWidgetItem();


        widget->setItem(row,0,itemNr);
        widget->setItem(row,1,itemName);
        widget->setItem(row,2,itemKlasse);
        widget->setItem(row,3,itemBetrieb);
        widget->setItem(row,4,itemAP);
        widget->setItem(row,5,itemMarker);

        itemNr->setTextColor(QColor(0,85,127,255));
        itemNr->setToolTip(tr("Prüfungsnummer"));
        itemName->setTextColor(QColor(0,85,127,255));

        itemNr->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemName->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemKlasse->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemBetrieb->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        itemAP->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

        itemAP->setData(Qt::FontRole, QFont(tableFont.family(),8));
        itemMarker->setCheckState(state);

        if(skillIsEvaluated){
           itemAP->setTextColor(Qt::darkGreen);
           itemAP->setToolTip(tr("Achtung bereits ausgewertet!\nBei neuer Zuordnung gehen alle ausgewerteten Daten verloren."));
       }else{
           itemAP->setTextColor(Qt::black);
           itemAP->setToolTip(tr("Zugeordente AP's"));
       }


        row++;

    }

    widget->resizeColumnToContents(0);
    widget->resizeColumnToContents(1);
    widget->resizeColumnToContents(2);
    widget->resizeColumnToContents(4);
    widget->resizeColumnToContents(5);

}

QMap<QString, ClassLehrling> FormAllocate::getSelectedApprentice(QTableWidget *widget)
{
    QMap<QString, ClassLehrling> aMap;

    int rows = widget->rowCount();
    for(int i = 0; i < rows; i++){
        if(widget->item(i, 5)->checkState() == Qt::Checked){
            QString key = widget->item(i, 1)->text();
            ClassLehrling app = apprenticeMap().value(key);
            aMap.insert(app.getKey(), app);
        }
    }

    return aMap;
}
