#include "fshkwindow.h"
#include "ui_fshkwindow.h"

#include <QSettings>
#include <QMessageBox>
#include <QFileInfo>
#include <QFile>
#include <QFileDevice>
#include <QDataStream>
#include <QFont>

#include <QDebug>

FSHKWindow::FSHKWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FSHKWindow)
{
    ui->setupUi(this);

    QString date = QDate::currentDate().toString("dd.MM.yyyy");
    QAction *action = new QAction(date);
    ui->menuBar->addAction(action);

    setWindowIcon(QIcon(":/FSHKIcon.svg"));
    readSettings();

    readDatas("Betriebe.dat");
    readDatas("Lehrlinge.dat");
    readDatas("Projekte.dat");
    readDatas("Pruefungen.dat");

    appwidget = new AppWidget(qApp->applicationName(), qApp->applicationVersion(), this);
    appwidget->hide();

    setApplicationLabel();

    formCompany = new FormBetrieb(this);
    formCompany->hide();
    connect(formCompany, &FormBetrieb::companyFormClosed, this, &FSHKWindow::formHasClosed);
    connect(formCompany, &FormBetrieb::saveCompanyMap, this, &FSHKWindow::saveCompanyMap);
    connect(formCompany, &FormBetrieb::companyRemoved, this, &FSHKWindow::companyRemoved);
    connect(formCompany, &FormBetrieb::apprenticeRemoved, this, &FSHKWindow::apprenticeRemoved);

    formApprentice = new FormLehrling(this);
    formApprentice->hide();
    connect(formApprentice, &FormLehrling::apprenticeFormClosed, this, &FSHKWindow::formHasClosed);
    connect(formApprentice, &FormLehrling::saveApprenticeMap, this, &FSHKWindow::saveApprenticeMap);
    connect(formApprentice, &FormLehrling::apprenticeWithoutCompany, this, &FSHKWindow::apprenticeWithoutCompany);
    connect(formApprentice, &FormLehrling::apprenticeHasCompany , this, &FSHKWindow::apprenticeAssociatedCompany );

    formProjekt = new FormProjekt(this);
    formProjekt->hide();
    connect(formProjekt, &FormProjekt::formProjectClosed, this, &FSHKWindow::formHasClosed);
    connect(formProjekt, &FormProjekt::saveProjekte, this, &FSHKWindow::saveProjectMap);
    connect(formProjekt, &FormProjekt::projektRemoved, this, &FSHKWindow::projectRemoved);
    connect(formProjekt, &FormProjekt::projektAdded, this, &FSHKWindow::projectAdded);
    connect(formProjekt, &FormProjekt::projektChanged, this, &FSHKWindow::projectChanged);

    formSkill = new FormSkills(this);
    formSkill->hide();
    connect(formSkill, &FormSkills::formSkillClosed, this, &FSHKWindow::formHasClosed);
    connect(formSkill, &FormSkills::saveSkillsMap, this, &FSHKWindow::saveSkillMap);
    connect(formSkill, &FormSkills::removeProjects, this, &FSHKWindow::removeProjects);

    formAllocate = new FormAllocate(this);
    formAllocate->hide();
    connect(formAllocate, &FormAllocate::formAllocateClosed, this, &FSHKWindow::formHasClosed);
    connect(formAllocate, &FormAllocate::saveApprenticeMap, this, &FSHKWindow::saveApprenticeMap);

    formEvaluation = new FormEvaluation(this);
    formEvaluation->hide();

    connect(ui->actionBeenden, &QAction::triggered, this, &FSHKWindow::actionCloseClicked);
    connect(ui->actionInfo, &QAction::triggered, this, &FSHKWindow::actionInfoClicked);
    connect(ui->actionCompany, &QAction::triggered, this, &FSHKWindow::actionCompanyClicked);
    connect(ui->actionApprentice, &QAction::triggered, this, &FSHKWindow::actionApprenticeClicked);
    connect(ui->actionProjects, &QAction::triggered, this, &FSHKWindow::actionProjectClicked);
    connect(ui->actionSkill, &QAction::triggered, this, &FSHKWindow::actionSkillClicked);
    connect(ui->actionAllocate, &QAction::triggered, this, &FSHKWindow::actionAllocateClicked);
    connect(ui->actionEvaluation, &QAction::triggered, this, &FSHKWindow::actionEvaluationClicked);

    setupMenu();

}

FSHKWindow::~FSHKWindow()
{
    delete ui;
}

void FSHKWindow::actionCloseClicked()
{
    writeSettings();
    close();
}

void FSHKWindow::actionInfoClicked()
{
    QMessageBox::information(this, tr("Info"), tr("Not available yet!"));
}

void FSHKWindow::actionCompanyClicked()
{
    this->takeCentralWidget();
    formCompany->setCompanyMap(companyMap);
    formCompany->setLastModified( lastFileModified("Betriebe.dat"));
    formCompany->show();
    setCentralWidget(formCompany);
}

void FSHKWindow::actionApprenticeClicked()
{
    this->takeCentralWidget();
    formApprentice->setApprenticeMap(apprenticeMap);
    formApprentice->setCompanyMap(companyMap);
    formApprentice->setLastModified( lastFileModified("Lehrlinge.dat"));
    formApprentice->show();
    setCentralWidget(formApprentice);
}

void FSHKWindow::actionProjectClicked()
{
    this->takeCentralWidget();
    formProjekt->show();
    formProjekt->setProjektMap(projectMap);
    setCentralWidget(formProjekt);
}

void FSHKWindow::actionSkillClicked()
{
    this->takeCentralWidget();
    formSkill->show();
    formSkill->setSkillMap(skillMap);
    formSkill->setProjektMap(projectMap);
    setCentralWidget(formSkill);
}

void FSHKWindow::actionAllocateClicked()
{
    this->takeCentralWidget();
    formAllocate->show();
    formAllocate->setSkillMap(skillMap);
    formAllocate->setApprenticeMap(apprenticeMap);
    setCentralWidget(formAllocate);
}

void FSHKWindow::actionEvaluationClicked()
{
    this->takeCentralWidget();
    formEvaluation->show();
    formEvaluation->setApprenticeMap( apprenticeMap );
    setCentralWidget(formEvaluation);
}

/// !brief This signal emitt when a form closed
/// to show the appWidget
void FSHKWindow::formHasClosed()
{
    setApplicationLabel();
}

/// Signals from FormCompany
void FSHKWindow::saveCompanyMap(const QMap<int, ClassBetrieb> &cMap)
{
    companyMap = cMap;
    saveDatas("Betriebe.dat");
    formCompany->setLastModified(lastFileModified("Betriebe.dat"));
}

/// !brief Check if any apprentice still in the company
void FSHKWindow::companyRemoved(const QString &company)
{
    bool changed = false;
    QMapIterator<QString, ClassLehrling> it(apprenticeMap);
    while (it.hasNext()) {
        it.next();
        ClassLehrling appr = it.value();
        if(appr.company() == company){
            appr.setCompany("");
            apprenticeMap.insert(appr.getKey(), appr);
            changed = true;
        }
    }

    if(changed){
        saveDatas("Lehrlinge.dat");
    }
}

/// !brief When an apprentice has been removed by company
/// remove the company from the apprentice
void FSHKWindow::apprenticeRemoved(const QString &company, const QString &apprenticeKey)
{
   ClassLehrling appr = apprenticeMap.value(apprenticeKey);
   if(appr.company() == company){
       appr.setCompany("");
       apprenticeMap.insert(appr.getKey(), appr);
       saveDatas("Lehrlinge.dat");
   }
}

/// !brief Signals from FormApprentice and FormAllocate
void FSHKWindow::saveApprenticeMap(const QMap<QString, ClassLehrling> &aMap)
{
    apprenticeMap = aMap;
    saveDatas("Lehrlinge.dat");
    formApprentice->setLastModified(lastFileModified("Lehrlinge.dat"));
    setupMenu();
}

/// !brief Signal emitter when editing an apprentice
/// and the company edit field is empty or the apprentice was deleting
void FSHKWindow::apprenticeWithoutCompany(const ClassLehrling &appr)
{
    bool dataChanged = false;
    QMapIterator<int, ClassBetrieb> it(companyMap);
    while (it.hasNext()) {
        it.next();
        ClassBetrieb comp = it.value();
        QMap<QString, ClassLehrling> aMap = comp.azubiMap();
        if(aMap.keys().contains(appr.getKey())){
            aMap.remove(appr.getKey());
            comp.setAzubiMap(aMap);
            companyMap.insert(comp.nr(), comp);
            dataChanged = true;
        }
    }

    if(dataChanged)
        saveDatas("Betriebe.dat");
}

/// !brief Signal emitter when editing an apprentice
/// and the company edit field was changed
void FSHKWindow::apprenticeAssociatedCompany(const QString &company, const QString &apprenticeKey)
{
    // Insert the apprentice into company
    ClassBetrieb comp = getCompany(company);
    ClassLehrling appr = apprenticeMap.value(apprenticeKey);
    QMap<QString, ClassLehrling> aMap = comp.azubiMap();
    aMap.insert(appr.getKey(), appr);
    comp.setAzubiMap(aMap);
    companyMap.insert(comp.nr(), comp);

    // Check if apprentice still in other company
    // if yes delete the apprentice from all other company
    QMapIterator<int, ClassBetrieb> it(companyMap);
    while (it.hasNext()) {
        it.next();
        if(it.value().azubiMap().keys().contains(apprenticeKey)){
            if(it.value().nr() != comp.nr()){
                ClassBetrieb co = it.value();
                QMap<QString, ClassLehrling> aMap = co.azubiMap();
                aMap.remove(apprenticeKey);
                co.setAzubiMap(aMap);
                companyMap.insert(co.nr(), co);
            }
        }

    }

    saveDatas("Betriebe.dat");
}

/// Signals from FormProject
void FSHKWindow::saveProjectMap(const QMap<QString, ClassProjekt> &pMap)
{
    projectMap = pMap;
    saveDatas("Projekte.dat");
}

/// !brief Check if any skill has the
/// removed project ask user to remove from skill
void FSHKWindow::projectRemoved(const ClassProjekt &pro)
{
    if(skillMap.isEmpty())
        return;

    ClassProjekt removedProject = pro;
    bool skillHasChanged = false;
    QMapIterator <QString, ClassSkills> it(skillMap);
    while (it.hasNext()) {
        it.next();
        ClassSkills skill = it.value();
        QMap< QString, ClassProjekt> pMap = skill.getProjektMap();

        if(pMap.keys().contains(removedProject.getKey())){

            int result = QMessageBox::question(this, tr("Projekt löschen"), tr("Soll das Projekt von der Prüfung:") + skill.getKey() + "\n" +
                                               tr("gelöscht werden?"), QMessageBox::No | QMessageBox::Yes);
            if(result == QMessageBox::Yes){
                pMap.remove(removedProject.getKey());
                skill.setProjektMap(pMap);
                skillMap.insert(it.key(), skill);
                skillHasChanged = true;
            }
        }
    }

    if(skillHasChanged)
        saveDatas("Pruefungen.dat");
}

/// !brief If skill has same identifier as the added
/// project then insert the project into the skill
void FSHKWindow::projectAdded(const ClassProjekt &pro)
{
    if(skillMap.isEmpty())
        return;

    ClassProjekt addedProject = pro;
    bool skillHasChanged = false;

    QMapIterator <QString, ClassSkills> it(skillMap);
    while (it.hasNext()) {
        it.next();
        ClassSkills skill = it.value();
        if(skill.identifier() == addedProject.identifier()){
            QMap< QString, ClassProjekt> pMap = skill.getProjektMap();
            pMap.insert(addedProject.getKey(), addedProject);
            skill.setProjektMap(pMap);
            skillMap.insert(it.key(), skill);
            skillHasChanged = true;
        }
    }

    if(skillHasChanged)
        saveDatas("Pruefungen.dat");
}

void FSHKWindow::projectChanged(const ClassProjekt &pro)
{
    if(skillMap.isEmpty())
        return;

    ClassProjekt addedProject = pro;
    bool skillHasChanged = false;

    QMapIterator <QString, ClassSkills> it(skillMap);
    while (it.hasNext()) {
        it.next();
        ClassSkills skill = it.value();
        if(skill.identifier() == addedProject.identifier()){
            QMap< QString, ClassProjekt> pMap = skill.getProjektMap();
            pMap.insert(addedProject.getKey(), addedProject);
            skill.setProjektMap(pMap);
            skillMap.insert(it.key(), skill);
            skillHasChanged = true;
        }
    }

    if(skillHasChanged)
        saveDatas("Pruefungen.dat");
}

/// !brief Signals from FormSkills
/// Updating the skill data
void FSHKWindow::saveSkillMap(const QMap<QString, ClassSkills> &sMap)
{
    skillMap = sMap;
    saveDatas("Pruefungen.dat");
    setupMenu();
}

/// !brief This signal emit after deleting skill when user
/// also wants to delete the projects from the skill
void FSHKWindow::removeProjects(const QMap<QString, ClassProjekt> &proMap)
{
    QMapIterator<QString, ClassProjekt> it(proMap);
    while (it.hasNext()) {
        it.next();
        if(projectMap.remove(it.key()) != 1){
            QMessageBox::information(this, tr("Projekt löschen"), tr("Das Projekt :" ) + it.key() + "\n" +
                                    tr( "konnte nicht gelöscht werden!\nMögliche Ursache:\nDas Projekt existiert nicht mehr."));
        }

    }

    saveDatas("Projekte.dat");
    formSkill->setProjektMap(projectMap);
}

/// !brief This function is called
/// when a Form has been closed
void FSHKWindow::setApplicationLabel()
{
    this->takeCentralWidget();
    appwidget->show();
    setCentralWidget(appwidget);
}

/// !brief Return ClassCompany seaching by name
ClassBetrieb FSHKWindow::getCompany(const QString &name)
{
    ClassBetrieb comp;
    QMapIterator<int, ClassBetrieb> it(companyMap);
    while (it.hasNext()) {
        it.next();
        if(it.value().name() == name){
            comp = it.value();
            break;
        }
    }

    return comp;
}

void FSHKWindow::readDatas(const QString &filename)
{
    QFile file(filename);
    if(!file.exists())
        return;

    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,  tr("Fehler Laden"),
                     tr("Die Datei: ")+ filename + tr("lässt sich nicht zum Lesen öffnen.\n") +
                      "Fehler: " + file.errorString());
        return;
    }

    QDataStream in(&file);
    quint32 magic;
    quint32 version;
    in >> magic;
    if(magic != 13101964){
        QMessageBox::warning(this, tr("Dateifehler"), tr("Die Datei hat nicht das richtige Format!"));
        return;
    }

    in >> version;
    if(version != QDataStream::Qt_5_9){
        QMessageBox::warning(this, tr("Dateifehler"), tr("Die Datei hat nicht die richtige Version!"));
        return;
    }

    while (!in.atEnd())
    {
        if(filename == "Betriebe.dat")
        {
            ClassBetrieb company;
            in >> company;
            companyMap.insert(company.nr(), company);
        }

        if(filename == "Lehrlinge.dat")
        {
            ClassLehrling appr;
            in >> appr;
            apprenticeMap.insert(appr.getKey(), appr);
        }
        if(filename == "Projekte.dat")
        {
            ClassProjekt pro;
            in >> pro;
            projectMap.insert(pro.getKey(), pro);
        }
        if(filename == "Pruefungen.dat")
        {
            ClassSkills skill;
            in >> skill;
            skillMap.insert(skill.getKey(), skill);
        }
    }

    file.close();
}

bool FSHKWindow::saveDatas(const QString &filename)
{
    QFile file(filename);

    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this,  tr("Fehler Laden"),
                     tr("Die Datei: ")+ filename + tr("lässt sich nicht zum Schreiben öffnen.\n") +
                      "Fehler: " + file.errorString());
        return false;
    }

    QDataStream out(&file);
    quint32 magic = 13101964;
    quint32 version = QDataStream::Qt_5_9;
    out << magic;
    out << version;

    if(filename == "Betriebe.dat")
    {
        QMapIterator<int, ClassBetrieb> it(companyMap);
        while (it.hasNext()) {
            it.next();
            out << it.value();
        }
    }
    if(filename == "Lehrlinge.dat")
    {
        QMapIterator<QString, ClassLehrling> it(apprenticeMap);
        while (it.hasNext()) {
            it.next();
            out << it.value();
        }
    }
    if(filename == "Projekte.dat")
    {
        QMapIterator<QString, ClassProjekt> it(projectMap);
        while (it.hasNext()) {
            it.next();
            out << it.value();
        }
    }
    if(filename == "Pruefungen.dat")
    {
        QMapIterator<QString, ClassSkills> it(skillMap);
        while (it.hasNext()) {
            it.next();
            out << it.value();
        }
    }

    file.close();
    return true;
}

QDateTime FSHKWindow::lastFileModified(const QString &filename)
{
    QDateTime dt;
    QFileInfo file(filename);
    dt = file.lastModified();
    return dt;
}

/// !brief Depends on datas to enable action menu
void FSHKWindow::setupMenu()
{
    if(skillMap.isEmpty())
        ui->actionAllocate->setEnabled(false);
    else
        ui->actionAllocate->setEnabled(true);

    // check if apprentice has any skills allocated
    bool skillsAllocated = false;
    QMapIterator <QString, ClassLehrling> it(apprenticeMap);
    while (it.hasNext()) {
        it.next();
        if( it.value().getSkillMap().size() > 0){
            skillsAllocated = true;
            break;
        }
    }

    if(skillsAllocated)
        ui->actionEvaluation->setEnabled(true);
    else
        ui->actionEvaluation->setEnabled(false);

}

/// !brief Read the settings
/// from last season
void FSHKWindow::readSettings()
{
    QSettings settings ("FASA-Soft", "FSHK-AP");
    QRect wRect = settings.value("geometrie", QRect(0,0,930,640)).toRect();
    this->setGeometry(wRect);

    QDateTime apprFileDate = settings.value("lehrling").toDateTime();
    if(lastFileModified("Lehrlinge.dat") != apprFileDate)
        QMessageBox::information(this, "Test", "Version Lehrlinge ist veraltet!");

    //qDebug() << "Read" << apprFileDate;
}

/// !brief Write the settings
/// from this season
void FSHKWindow::writeSettings()
{
    QSettings settings ("FASA-Soft", "FSHK-AP");
    settings.setValue("geometrie", this->geometry());
    settings.setValue("lehrling", lastFileModified("Lehrlinge.dat"));
    settings.setValue("betrieb", lastFileModified("Betriebe.dat"));


    //qDebug() << "Write" << lastFileModified("Betriebe.dat");
}
