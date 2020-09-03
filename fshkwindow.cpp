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
    connect(formCompany, &FormBetrieb::companyFormClosed, this, &FSHKWindow::formHasClosed);            // Works
    connect(formCompany, &FormBetrieb::saveCompanyMap, this, &FSHKWindow::saveCompanyMap);              // Works
    connect(formCompany, &FormBetrieb::companyRemoved, this, &FSHKWindow::companyRemoved);              // Works
    connect(formCompany, &FormBetrieb::apprenticeRemoved, this, &FSHKWindow::apprenticeRemoved);        // Works

    formApprentice = new FormLehrling(this);
    formApprentice->hide();
    connect(formApprentice, &FormLehrling::apprenticeFormClosed, this, &FSHKWindow::formHasClosed);     // Works
    connect(formApprentice, &FormLehrling::saveApprenticeMap, this, &FSHKWindow::saveApprenticeMap);    // Works
    connect(formApprentice, &FormLehrling::apprenticeWithoutCompany, this, &FSHKWindow::apprenticeWithoutCompany);      // Works
    connect(formApprentice, &FormLehrling::apprenticeHasCompany , this, &FSHKWindow::apprenticeAssociatedCompany );     // Works

    formProjekt = new FormProjekt(this);
    formProjekt->hide();
    connect(formProjekt, &FormProjekt::formProjectClosed, this, &FSHKWindow::formHasClosed);    // Works
    connect(formProjekt, &FormProjekt::saveProjekte, this, &FSHKWindow::saveProjectMap);        // Works
    connect(formProjekt, &FormProjekt::projektRemoved, this, &FSHKWindow::projectRemoved);      // Works
    connect(formProjekt, &FormProjekt::projektAdded, this, &FSHKWindow::projectAdded);          // Works
    connect(formProjekt, &FormProjekt::projektChanged, this, &FSHKWindow::projectChanged);      // Works

    formSkill = new FormSkills(this);
    formSkill->hide();
    connect(formSkill, &FormSkills::formSkillClosed, this, &FSHKWindow::formHasClosed);         // Works
    connect(formSkill, &FormSkills::saveSkillsMap, this, &FSHKWindow::saveSkillMap);            // Works
    connect(formSkill, &FormSkills::removeProjects, this, &FSHKWindow::removeProjects);
    connect(formSkill, &FormSkills::skillChanged, this, &FSHKWindow::skillChanged);

    formAllocate = new FormAllocate(this);
    formAllocate->hide();
    connect(formAllocate, &FormAllocate::formAllocateClosed, this, &FSHKWindow::formHasClosed);
    connect(formAllocate, &FormAllocate::saveApprenticeMap, this, &FSHKWindow::saveApprenticeMap);

    formEvaluation = new FormEvaluation(this);
    formEvaluation->hide();
    connect(formEvaluation, &FormEvaluation::saveApprenticeMap, this, &FSHKWindow::saveApprenticeMap);

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
    //updateCompanyData();
}

/// !brief Check if any apprentice still in the company
void FSHKWindow::companyRemoved(const QString &company)
{
    foreach (ClassLehrling azu, apprenticeMap.values()) {
        if(azu.company() == company){
            azu.setCompany("");
            apprenticeMap.insert(azu.getKey(), azu);
            saveDatas("Lehrlinge.dat");
        }
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
    //updateApprenticeData();
}

/// !brief Signal emitt when editing an apprentice
/// and the company edit field is empty or the apprentice was deleting
void FSHKWindow::apprenticeWithoutCompany(const QString &apprKey)
{    
    foreach (ClassBetrieb comp, companyMap.values()) {
        if(comp.azuKeys().contains( apprKey )){
            comp.removeApprentice(apprKey);
            companyMap.insert(comp.nr(), comp);
            saveDatas("Betriebe.dat");
        }
    }
}

/// !brief Signal emitt when editing an apprentice
/// and the company edit field was changed
void FSHKWindow::apprenticeAssociatedCompany(const QString &company, const QString &apprenticeKey)
{
    // Insert the apprentice into company
    ClassBetrieb comp = getCompany(company);
    ClassLehrling appr = apprenticeMap.value(apprenticeKey);
    comp.insertApprentice(appr);
    companyMap.insert(comp.nr(), comp);

    foreach (ClassBetrieb c, companyMap.values()) {
        if(c.azuKeys().contains(apprenticeKey) && c.name() != comp.name()){
            c.removeApprentice(apprenticeKey);
            companyMap.insert(c.nr(), c);
        }
    }

    saveDatas("Betriebe.dat");
}

/// Signals from FormProject
///
void FSHKWindow::saveProjectMap(const QMap<QString, ClassProjekt> &pMap)
{
    projectMap = pMap;
    saveDatas("Projekte.dat");
 //   updateProjektData();

}

/// !brief Check if any skill has the
/// removed project ask user to remove from skill
void FSHKWindow::projectRemoved(const QString &proKey)
{
    if(skillMap.isEmpty())
        return;

    // Delete projekt from skill and store the changed skill
    QStringList skillKeyList;
    foreach (ClassSkills s, skillMap.values()) {
        if(s.containsProject( proKey )){
            s.removeProject(proKey);
            skillMap.insert(s.getKey(),s);
            skillKeyList << s.getKey();
        }
    }

    saveDatas("Pruefungen.dat");

    if(skillKeyList.isEmpty())
        return;

    updateApprenticeSkillData(skillKeyList, proKey, "remove");

//    QMap<QString, QVariant> azuSkillMap;
//    foreach (ClassLehrling azu, apprenticeMap.values())
//    {
//        for(int i = 0; i < skillKeyList.size(); i++)
//        {
//            if(azu.containsSkill(skillKeyList.at(i)))
//            {
//                if(azu.isSkillEvaluated(skillKeyList.at(i)))
//                    azuSkillMap.insert(azu.getKey(), "evaluatedSkill");
//                else
//                    azuSkillMap.insert(azu.getKey(), "cleanSkill");
//            }
//        }
//    }

//    if(azuSkillMap.isEmpty())
//        return;

//    int result = QMessageBox::question(this, tr("Prüfungen"), tr("Die geänderte Prüfung wurde einige Auszubildenden zugeordnet."
//                                    "Sollen die Prüfungen auch geändert werden?"), QMessageBox::No | QMessageBox::Yes);

//    if(result == QMessageBox::No)
//        return;

//    QStringList azuKeyList;
//    azuKeyList << azuSkillMap.keys("cleanSkill");

//    if(!azuSkillMap.keys("evaluatedSkill").isEmpty())
//    {
//        QString title = "Auszubildende mit geänderten Prüfungen";
//        QString message = "Bei den unten aufgeführten Auszubildenden wurde die Prüfung bereits "
//                          "ganz oder teils ausgewertet. Durch die Änderung können Ausgewertete Daten verloren gehen!";

//        DialogApprenticeList *dlg = new  DialogApprenticeList( title, message, azuSkillMap.keys("evaluatedSkill") ,this);

//        if(dlg->exec() == QDialog::Accepted)
//            azuKeyList << azuSkillMap.keys("evaluatedSkill");;

//    }


//    foreach (QString key, azuKeyList)
//    {
//        ClassLehrling appr = apprenticeMap.value(key);
//        for(int i = 0; i < skillKeyList.size(); i++)
//        {
//            if(!appr.isSkillEvaluated(skillKeyList.at(i)))
//                appr.insertSkill( skillMap.value(skillKeyList.at(i)));
//            else {
//                ;
//            }

//            apprenticeMap.insert(appr.getKey(), appr);
//        }
//    }



//    saveDatas("Lehrlinge.dat");


//    // Remove project from skill if skill has same identifier
//    // Returns the changed skills key
//    QStringList skillList = removeProjectFromSkill(pro);

//    // Check if any apprentice has this changed skill
//    bool apprenticeDataChanged = false;
//    ClassProjekt project = pro;
//    foreach (QString skey, skillList)
//    {

//        QMap<QString, QVariant> keyMap = getApprenticeKeyMap( skey );

//        // All apprentices with the changed skill (not evaluated)
//        QStringList cleanList = keyMap.keys("cleanSkill");
//        foreach (QString apprKey, cleanList){

//            ClassLehrling appr = apprenticeMap.value(apprKey);
//            if(appr.removeProject( project.getKey() ))
//                apprenticeMap.insert(appr.getKey(), appr);
//        }

//        // All apprentices with the changed skill (evaluated)
//        QStringList evaluatedList = keyMap.keys("evaluatedSkill");
//        if(!evaluatedList.isEmpty())
//        {
//            QString title = "Auszubildende mit Prüfungen";
//            QString message = "Bei den unten aufgeführten Auszubildenden wurde die Prüfung bereits "
//                              "ganz oder teils ausgewertet. Ausgewertete Daten könnten verloren gehen!";
//            DialogApprenticeList *dlg = new  DialogApprenticeList(title,message, evaluatedList ,this);
//            if(dlg->exec() == QDialog::Accepted)
//            {
//                foreach (QString apprKey, evaluatedList)
//                {
//                    ClassLehrling appr = apprenticeMap.value(apprKey);
//                    if(appr.removeProject( project.getKey() ))
//                        apprenticeMap.insert(appr.getKey(), appr);
//                }

//            }
//        }

//        if(!cleanList.isEmpty() || !evaluatedList.isEmpty())
//            apprenticeDataChanged = true;

//    }

//    if(apprenticeDataChanged)
//        saveDatas("Lehrlinge.dat");


}

/// !brief If skill has same identifier as the added
/// project then insert the project into the skill
void FSHKWindow::projectAdded(const ClassProjekt &pro)
{
    if(skillMap.isEmpty())
        return;

    ClassProjekt project = pro;

    bool skillDataChanged = false;
    QStringList skillKeyList;
    foreach (ClassSkills sk, skillMap.values())
    {
        if(sk.identifier() == pro.identifier())
        {
            sk.insertProjekt(pro);
            skillMap.insert(sk.getKey(), sk);
            skillKeyList << sk.getKey();
            skillDataChanged = true;
        }
    }

    if(skillDataChanged)
        saveDatas("Pruefungen.dat");

    if(skillKeyList.isEmpty())
        return;

    updateApprenticeSkillData(skillKeyList, project.getKey(), "add");

//    if(!skillSameIdentifier(pro.identifier()))
//        return;

//    // Add project to skill if skill has same identifier
//    // Returns the changed skills key
//    QStringList skillList = addProjectToSkill(pro);

//    // Check if any apprentice has this changed skill
//    bool apprenticeDataChanged = false;
//    foreach (QString skey, skillList)
//    {

//        QMap<QString, QVariant> keyMap = getApprenticeKeyMap( skey );

//        // All apprentices with the changed skill (not evaluated)
//        QStringList cleanList = keyMap.keys("cleanSkill");
//        foreach (QString apprKey, cleanList)
//            addProjectToApprentice(apprKey, skey, pro);

//        // All apprentices with the changed skill (evaluated)
//        QStringList evaluatedList = keyMap.keys("evaluatedSkill");
//        if(!evaluatedList.isEmpty())
//        {
//            QString title = "Auszubildende mit Prüfungen";
//            QString message = "Bei den unten aufgeführten Auszubildenden wurde die Prüfung bereits "
//                              "ganz oder teils ausgewertet. Ausgewertete Daten könnten verloren gehen!";
//            DialogApprenticeList *dlg = new  DialogApprenticeList(title,message, evaluatedList ,this);
//            if(dlg->exec() == QDialog::Accepted)
//            {
//                foreach (QString apprKey, evaluatedList)
//                    addProjectToApprentice(apprKey, skey, pro);

//            }
//        }

//        if(!cleanList.isEmpty() || !evaluatedList.isEmpty())
//            apprenticeDataChanged = true;

//    }

//    if(apprenticeDataChanged)
//        saveDatas("Lehrlinge.dat");
}

void FSHKWindow::projectChanged(const ClassProjekt &pro)
{
    if(skillMap.isEmpty())
        return;

    ClassProjekt project = pro;

    bool skillDataChanged = false;
    QStringList skillKeyList;
    foreach (ClassSkills sk, skillMap.values())
    {
        if(sk.identifier() == pro.identifier())
        {
            sk.insertProjekt(pro);
            skillMap.insert(sk.getKey(), sk);
            skillKeyList << sk.getKey();
            skillDataChanged = true;
        }
    }

    if(skillDataChanged)
        saveDatas("Pruefungen.dat");

    if(skillKeyList.isEmpty())
        return;

    updateApprenticeSkillData(skillKeyList, project.getKey(), "add");

//    if(!skillSameIdentifier(pro.identifier()))
//        return;

//    // Add project to skill if skill has same identifier
//    // Returns the changed skills key
//    QStringList skillList = addProjectToSkill(pro);

//    // Check if any apprentice has this changed skill
//    bool apprenticeDataChanged = false;
//    foreach (QString skey, skillList)
//    {

//        QMap<QString, QVariant> keyMap = getApprenticeKeyMap( skey );

//        // All apprentices with the changed skill (not evaluated)
//        QStringList cleanList = keyMap.keys("cleanSkill");
//        foreach (QString apprKey, cleanList)
//            addProjectToApprentice(apprKey, skey, pro);

//        // All apprentices with the changed skill (evaluated)
//        QStringList evaluatedList = keyMap.keys("evaluatedSkill");
//        if(!evaluatedList.isEmpty())
//        {
//            QString title = "Auszubildende mit Prüfungen";
//            QString message = "Bei den unten aufgeführten Auszubildenden wurde die Prüfung bereits "
//                              "ganz oder teils ausgewertet. Ausgewertete Daten könnten verloren gehen!";
//            DialogApprenticeList *dlg = new  DialogApprenticeList(title,message, evaluatedList ,this);
//            if(dlg->exec() == QDialog::Accepted)
//            {
//                foreach (QString apprKey, evaluatedList)
//                    addProjectToApprentice(apprKey, skey, pro);

//            }
//        }

//        if(!cleanList.isEmpty() || !evaluatedList.isEmpty())
//            apprenticeDataChanged = true;

//    }

//    if(apprenticeDataChanged)
//        saveDatas("Lehrlinge.dat");


//    insertProjectToSkill(pro);
//    insertProjectToApprentice(pro);
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

void FSHKWindow::skillChanged(const ClassSkills &skill)
{
    // Check if any apprentice evaluated this skill
//    QStringList azuList = getApprenticeList(skill);
//    QStringList azuEvaluatedList;

//    ClassSkills sk = skill;
//    foreach (QString key, azuList) {
//        if(apprenticeMap.value(key).isSkillEvaluated(sk.getKey()))
//            azuEvaluatedList << key;
//    }

//    if(!azuList.isEmpty()){
//        bool dataChanged = false;
//        QString message = "Bei den unten aufgeführten Auszubildenden wurde die geänderte Prüfung teils oder ganz Ausgewertet."
//                          "Wenn die Prüfung bei den Auszubildenden geändert wird kann die Auswertung verloren gehen!";
//        QString title =  "Auszubildende mit ausgewerteten Prüfungen";
//        DialogApprenticeList *formDlg = new DialogApprenticeList(title, message, azuList, this);
//        if(formDlg->exec() == QDialog::Rejected)
//            return;

//        ClassSkills nsk = skill;                                        // The changed skill
//        foreach (QString key, azuList) {

//            ClassLehrling appr = apprenticeMap.value(key);
//            ClassSkills osk = appr.getSkillMap().value(nsk.getKey());   // The old skill from apprentice

//            if(!osk.isEvaluated())
//                osk = nsk;
//            else
//            {
//                // Try to not overwrite the hole skill
//                // Compare the project instead...
//                foreach (ClassProjekt pro, nsk.getProjektMap().values())    // Compare each project
//                {
//                    if(!osk.containsProject(pro.getKey()))                  // If project not exist
//                    {
//                        osk.insertProjekt(pro);                             // Insert the new project into old skill
//                    }
//                    else
//                    {                                                  // If exist

//                        if(!osk.getProjektMap().value(pro.getKey()).getEvaluated() ) // If project is not evaluated
//                            osk.insertProjekt(pro);                         // Insert the new project into old skill
//                        else                                                // If project is evaluated
//                        {
//                            // Check created date
//                            QDateTime proDT = QDateTime::fromString( pro.createTime(), "dd.MM.yy hh:mm" );
//                            QDateTime oldproDT = QDateTime::fromString( osk.getProjektMap().value(pro.getKey()).createTime(), "dd.MM.yy hh:mm" );
//                            QDateTime now = QDateTime::currentDateTime();
//                            if(proDT.secsTo(now) < oldproDT.secsTo(now))
//                                osk.insertProjekt(pro);
//                        }
//                    }
//                }
//            }

//            appr.insertSkill(osk);
//            apprenticeMap.insert(appr.getKey(), appr);
//            dataChanged = true;

//        }

//        if(dataChanged)
//            saveDatas("Lehrlinge.dat");

    //    }
}

QStringList FSHKWindow::skillKeyList(const QString &proIdentifier)
{
    QStringList keyList;
    foreach (ClassSkills sk, skillMap.values()) {
        if(sk.identifier() == proIdentifier)
            keyList << sk.getKey();
    }
    return keyList;
}

bool FSHKWindow::skillSameIdentifier(const QString &proIdent)
{
    bool status = false;

    QMapIterator<QString, ClassSkills> it(skillMap);
    while (it.hasNext()) {
        it.next();
        if(it.value().identifier() == proIdent){
            status = true;
            break;
        }
    }

    return status;
}

QStringList FSHKWindow::addProjectToSkill(const ClassProjekt &pro)
{
    QStringList skillKey;
    ClassProjekt project = pro;
    QMapIterator<QString, ClassSkills> it(skillMap);
    while (it.hasNext()) {
        it.next();
        if(it.value().identifier() == project.identifier()){
            ClassSkills sk = it.value();
            sk.insertProjekt(project);
            skillMap.insert(sk.getKey(), sk);

            // Store the changed skills
            skillKey << sk.getKey();
        }
    }
    saveDatas("Pruefungen.dat");
    return skillKey;
}

QStringList FSHKWindow::removeProjectFromSkill(const ClassProjekt &pro)
{
    QStringList skillKey;
    ClassProjekt project = pro;
    QMapIterator<QString, ClassSkills> it(skillMap);
    while (it.hasNext())
    {
        it.next();
        ClassSkills sk = it.value();
        if(sk.containsProject( project.getKey() ))
        {
            if(sk.removeProject( project.getKey() ))
            {
                skillMap.insert(sk.getKey(), sk);
                skillKey << sk.getKey();

            }
        }
    }
    saveDatas("Pruefungen.dat");
    return skillKey;
}

void FSHKWindow::addProjectToApprentice(const QString &apprKey, const QString &skillKey, const ClassProjekt &pro)
{
    ClassProjekt project = pro;
    ClassLehrling appr = apprenticeMap.value(apprKey);
    QMap<QString, ClassSkills> sMap = appr.getSkillMap();
    ClassSkills skill = sMap.value(skillKey);
    skill.insertProjekt(project);
    sMap.insert(skill.getKey(), skill);
    appr.setSkillMap(sMap);
    apprenticeMap.insert(appr.getKey(), appr);
}

//bool FSHKWindow::skillListContainsProject(const QString &proKey)
//{
//    bool status = false;

//    QMapIterator<QString, ClassSkills> it(skillMap);
//    while (it.hasNext()) {
//        it.next();
//        if(it.value().identifier() == proKey){
//            status = true;
//            break;
//        }
//    }

//    return status;
//}

QMap<QString, QVariant> FSHKWindow::getApprenticeKeyMap(const QString &skillKey)
{
    QMap<QString, QVariant> keyMap;
    QMapIterator<QString, ClassLehrling> it(apprenticeMap);
    while (it.hasNext()) {
        it.next();
        ClassLehrling appr = it.value();
        if (appr.containsSkill(skillKey) && appr.isSkillEvaluated(skillKey) )
                keyMap.insert(appr.getKey(), "evaluatedSkill");

        if (appr.containsSkill(skillKey) && !appr.isSkillEvaluated(skillKey) )
                keyMap.insert(appr.getKey(), "cleanSkill");

    }
    return keyMap;
}

//QStringList FSHKWindow::getApprenticeList(const ClassSkills &skill)
//{
//    ClassSkills sk = skill;
//    QStringList list;
//    QMapIterator<QString, ClassLehrling> it(apprenticeMap);
//    while (it.hasNext()) {
//        it.next();
//        ClassLehrling appr = it.value();
//        if(appr.containsSkill(sk.getKey()))
//                list << appr.getKey();
//    }
//    return list;
//}

//void FSHKWindow::insertProjectToSkill(const ClassProjekt &pro)
//{
//    QList<ClassSkills> skillList;
//    bool dataChanged = false;
//    ClassProjekt project = pro;
//    QMapIterator <QString, ClassSkills> it(skillMap);
//    while (it.hasNext()) {
//        it.next();
//        ClassSkills skill = it.value();
//        if(skill.containsProject( project.getKey() )){
//            skill.insertProjekt(project);
//            skillMap.insert(it.key(), skill);
//            skillList << skill;
//            dataChanged = true;
//        }
//    }

//    if(dataChanged){
//        saveDatas("Pruefungen.dat");

//        foreach (ClassSkills sk, skillList) {
//            skillChanged(sk);
//        }
//    }
//}

//void FSHKWindow::addProjectToSkill(const ClassProjekt &pro)
//{
//    QList<ClassSkills> skillList;
//    bool dataChanged = false;
//    ClassProjekt project = pro;
//    QMapIterator <QString, ClassSkills> it(skillMap);
//    while (it.hasNext()) {
//        it.next();
//        ClassSkills skill = it.value();
//        if(skill.identifier() == pro.identifier()){
//            skill.insertProjekt(project);
//            skillMap.insert(it.key(), skill);
//            skillList << skill;
//            dataChanged = true;
//        }
//    }

//    if(dataChanged){
//        saveDatas("Pruefungen.dat");

//        foreach (ClassSkills sk, skillList) {
//            skillChanged(sk);
//        }
//    }
//}

//void FSHKWindow::insertProjectToApprentice(const ClassProjekt &pro)
//{
//    bool apprenticeChanged = false;
//    ClassProjekt project = pro;
//    QMapIterator <QString, ClassLehrling> it(apprenticeMap);
//    while (it.hasNext()) {
//        it.next();
//        ClassLehrling appr = it.value();
//        QMap<QString, ClassSkills> sMap = appr.getSkillMap();
//        foreach (ClassSkills skill, sMap.values()) {
//            if(skill.containsProject( project.getKey() )){
//                skill.insertProjekt(project);
//                sMap.insert(skill.getKey(), skill);
//                appr.setSkillMap(sMap);
//                apprenticeMap.insert(appr.getKey(), appr);
//                apprenticeChanged = true;
//            }
//        }
//    }

//    if(apprenticeChanged)
//        saveDatas("Lehrlinge.dat");
//}

//void FSHKWindow::addProjectToApprentice(const ClassProjekt &pro)
//{
//    bool apprenticeChanged = false;
//    ClassProjekt project = pro;
//    QMapIterator <QString, ClassLehrling> it(apprenticeMap);
//    while (it.hasNext()) {
//        it.next();
//        ClassLehrling appr = it.value();
//        QMap<QString, ClassSkills> sMap = appr.getSkillMap();
//        foreach (ClassSkills skill, sMap.values()) {
//            if(skill.identifier() == project.identifier()){
//                skill.insertProjekt(project);
//                sMap.insert(skill.getKey(), skill);
//                appr.setSkillMap(sMap);
//                apprenticeMap.insert(appr.getKey(), appr);
//                apprenticeChanged = true;
//            }
//        }
//    }

//    if(apprenticeChanged)
//        saveDatas("Lehrlinge.dat");
//}

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

void FSHKWindow::updateCompanyData()
{
    bool apprenticeDataChanged = false;
    bool companyDataChanged = false;

    QMapIterator<int, ClassBetrieb> it(companyMap);
    while (it.hasNext())
    {
        it.next();
        ClassBetrieb comp = it.value();

        QStringList azuKeyList = comp.azuKeys();
        QMapIterator<QString, ClassLehrling> ip(apprenticeMap);
        while (ip.hasNext())
        {
            ip.next();
            ClassLehrling appr = ip.value();

            // Company has no apprentice
            if(appr.company() == comp.name() && azuKeyList.isEmpty())
                appr.setCompany("");

            // When company was removed
            if(!companyKeyList().contains(appr.company()))
                appr.setCompany("");

            // When company has not this apprentice ...
            if(!comp.azuKeys().contains( appr.getKey() ) && appr.company() == comp.name())
                appr.setCompany("");

            apprenticeMap.insert(appr.getKey(), appr);
            apprenticeDataChanged = true;

        }

        foreach (QString key, comp.azuKeys())
        {
            if(!apprenticeExist(key))
            {
                comp.removeApprentice(key);
                companyMap.insert(comp.nr(), comp);
                companyDataChanged = true;
            }

        }
    }


    if(apprenticeDataChanged)
        saveDatas("Lehrlinge.dat");

    if(companyDataChanged)
        saveDatas("Betriebe.dat");
}

QStringList FSHKWindow::companyKeyList()
{
    QStringList list;
    foreach (ClassBetrieb b, companyMap.values()) {
        list << b.name();
    }
    return list;
}

void FSHKWindow::updateApprenticeSkillData(const QStringList &skillKeyList, const QString &proKey ,const QString &order)
{
    QMap<QString, QVariant> azuSkillMap;
    foreach (ClassLehrling azu, apprenticeMap.values())
    {
        for(int i = 0; i < skillKeyList.size(); i++)
        {
            if(azu.containsSkill(skillKeyList.at(i)))
            {
                if(azu.isSkillEvaluated(skillKeyList.at(i)))
                    azuSkillMap.insert(azu.getKey(), "evaluatedSkill");
                else
                    azuSkillMap.insert(azu.getKey(), "cleanSkill");
            }
        }
    }

    if(azuSkillMap.isEmpty())
        return;

    int result = QMessageBox::question(this, tr("Prüfungen"), tr("Die geänderte Prüfung wurde einige Auszubildenden zugeordnet."
                                    "Sollen die Prüfungen auch geändert werden?"), QMessageBox::No | QMessageBox::Yes);

    if(result == QMessageBox::No)
        return;

    QStringList azuKeyList;
    azuKeyList << azuSkillMap.keys("cleanSkill");

    if(!azuSkillMap.keys("evaluatedSkill").isEmpty())
    {
        QString title = "Auszubildende mit geänderten Prüfungen";
        QString message = "Bei den unten aufgeführten Auszubildenden wurde die Prüfung bereits "
                          "ganz oder teils ausgewertet. Durch die Änderung können Ausgewertete Daten verloren gehen!";

        DialogApprenticeList *dlg = new  DialogApprenticeList( title, message, azuSkillMap.keys("evaluatedSkill") ,this);

        if(dlg->exec() == QDialog::Accepted)
            azuKeyList << azuSkillMap.keys("evaluatedSkill");;

    }


    foreach (QString key, azuKeyList)
    {
        ClassLehrling appr = apprenticeMap.value(key);
        for(int i = 0; i < skillKeyList.size(); i++)
        {
            if(!appr.isSkillEvaluated(skillKeyList.at(i)))
                appr.insertSkill( skillMap.value(skillKeyList.at(i)));
            else {
                 ClassSkills sk = appr.getSkill( skillKeyList.at(i));
                 if(order == "remove")
                     sk.removeProject(proKey);

                 if(order == "add"){
                     ClassProjekt pro = projectMap.value(proKey);
                     sk.insertProjekt(pro);
                 }

                 appr.insertSkill(sk);
            }

            apprenticeMap.insert(appr.getKey(), appr);
        }
    }



    saveDatas("Lehrlinge.dat");

}

void FSHKWindow::updateApprenticeData()
{
    bool apprenticeDataChanged = false;
    bool companyDataChanged = false;

    // Get some values
    QStringList apprNames;

    QMapIterator<QString, ClassLehrling> it(apprenticeMap);
    while (it.hasNext())
    {
        it.next();
        ClassLehrling appr = it.value();

        // Apprentice without company
        if(appr.company().isEmpty())
        {
            foreach (ClassBetrieb co, companyMap.values())
            {
                if(co.azuKeys().contains( appr.getKey() ))
                {
                    if(co.removeApprentice(appr.getKey()))
                    {
                        companyMap.insert(co.nr(), co);
                        companyDataChanged = true;
                    }
                }
            }
        }

        if(!appr.company().isEmpty())
        {
           ClassBetrieb co = getCompany( appr.company() );
           if(!co.azuKeys().contains(appr.getKey()))
           {
               co.insertApprentice(appr);
               companyMap.insert(co.nr(), co);
               companyDataChanged = true;
           }
        }

        // Update skills in apprentice
        foreach (ClassSkills sk, skillMap.values())
        {
            if(appr.containsSkill(sk.getKey()))
            {
                if(!appr.isSkillEvaluated(sk.getKey()))
                {
                    appr.insertSkill(sk);
                    apprenticeMap.insert(appr.getKey(), appr);
                    apprenticeDataChanged = true;
                }

                if(appr.isSkillEvaluated(sk.getKey()))
                {
                    if(!appSettingsMap.value("overwriteEvaluatedSkills").toBool())
                        apprNames << appr.getKey();
                    else
                    {
                        appr.insertSkill(sk);
                        apprenticeMap.insert(appr.getKey(), appr);
                        apprenticeDataChanged = true;
                    }
                }
            }
        }
    }

    if(!apprNames.isEmpty())
    {
        QString title = "Auszubildende mit Prüfungen";
        QString message = "Bei den unten aufgeführten Auszubildenden wurde die Prüfung bereits "
                         "ganz oder teils ausgewertet. Ausgewertete Daten könnten verloren gehen!";
        DialogApprenticeList *dlg = new  DialogApprenticeList(title,message, apprNames ,this);
        if(dlg->exec() == QDialog::Accepted)
        {
            appSettingsMap.insert("overwriteEvaluatedSkills", true);
            updateApprenticeData();
        }
    }


    if(companyDataChanged)
        saveDatas("Betriebe.dat");

    if(apprenticeDataChanged)
        saveDatas("Pruefungen.dat");

    appSettingsMap.insert("overwriteEvaluatedSkills", false);

}

bool FSHKWindow::apprenticeExist(const QString &key)
{
    bool status = false;
    if(!apprenticeMap.value(key).surname().isEmpty())
        status = true;
    return status;
}

void FSHKWindow::updateProjektData()
{
    if(skillMap.isEmpty())
        return;

    bool skillDataChanged = false;
    QMapIterator<QString, ClassProjekt> it(projectMap);
    while (it.hasNext())
    {
        it.next();

        ClassProjekt pro = it.value();

        // Copy all projects into the skill with the same identifier
        foreach (ClassSkills sk, skillMap.values())
        {

            if(sk.identifier() == pro.identifier())
            {
                sk.insertProjekt(pro);
                skillMap.insert(sk.getKey(), sk);
                skillDataChanged = true;
            }
        }  
    }

    foreach (ClassSkills sk, skillMap.values())
    {
        QStringList skillProjectKeyList = sk.projectKeyList();
        for(int i = 0; i < skillProjectKeyList.size(); i++)
        {
            if(!projectKeyList(sk.identifier()).contains(skillProjectKeyList.at(i)))
            {
                sk.removeProject(skillProjectKeyList.at(i));
                skillMap.insert(sk.getKey(), sk);
                skillDataChanged = true;

            }
        }
    }

    if(skillDataChanged){
        saveDatas("Pruefungen.dat");
        updateApprenticeData();
    }


}

QStringList FSHKWindow::projectKeyList(const QString &identifier)
{
    QStringList keyList;
    QMapIterator<QString, ClassProjekt> it(projectMap);
    while (it.hasNext()) {
        it.next();
        ClassProjekt pro = it.value();
        if(pro.identifier() == identifier)
            keyList << pro.getKey();
    }

    return keyList;
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

//    QDateTime apprFileDate = settings.value("lehrling").toDateTime();
//    if(lastFileModified("Lehrlinge.dat") != apprFileDate)
//        QMessageBox::information(this, "Test", "Version Lehrlinge ist veraltet!");

     appSettingsMap.insert("overwriteEvaluatedSkills", false);
}

/// !brief Write the settings
/// from this season
void FSHKWindow::writeSettings()
{
    QSettings settings ("FASA-Soft", "FSHK-AP");
    settings.setValue("geometrie", this->geometry());
    settings.setValue("lehrling", lastFileModified("Lehrlinge.dat"));
    settings.setValue("betrieb", lastFileModified("Betriebe.dat"));


}
