#include "fshkwindow.h"
#include "ui_fshkwindow.h"

#include <QSettings>
#include <QMessageBox>
#include <QFileInfo>
#include <QFile>
#include <QFileDevice>
#include <QDataStream>
#include <QPrintDialog>
#include <QPrinter>
#include <QPainter>
#include <QFont>
#include <QPageSize>
#include <QPageLayout>
#include <QtMath>
#include <QDir>

#include <QDebug>


#include <dialoginfo.h>
#include <classprinting.h>
#include <dialogappsettings.h>

FSHKWindow::FSHKWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FSHKWindow)
{
    ui->setupUi(this);

    QString date = QDate::currentDate().toString("dd.MM.yyyy");
    QAction *action = new QAction(date);
    ui->menuBar->addAction(action);

    setWindowIcon(QIcon(":/images/FSHKIcon.ico"));
    readSettings();

    readDatas("Betriebe.dat");
    readDatas("Lehrlinge.dat");
    readDatas("Projekte.dat");
    readDatas("Pruefungen.dat");

    appwidget = new AppWidget(qApp->applicationName(), qApp->applicationVersion(), this);
    appwidget->hide();

    setApplicationLabel();


    dialogAppSettings = new DialogAppSettings(appSettingsMap, this);


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
    connect(formApprentice, &FormLehrling::printApprentice , this, &FSHKWindow::printApprentice );
    connect(formApprentice, &FormLehrling::removeFromCompany , this, &FSHKWindow::removeFromCompany );

    formProjekt = new FormProjekt(this);
    formProjekt->hide();
    connect(formProjekt, &FormProjekt::formProjectClosed, this, &FSHKWindow::formHasClosed);    // Works
    connect(formProjekt, &FormProjekt::saveProjekte, this, &FSHKWindow::saveProjectMap);        // Works
    connect(formProjekt, &FormProjekt::projektRemoved, this, &FSHKWindow::projectRemoved);      // Works
    connect(formProjekt, &FormProjekt::projektAdded, this, &FSHKWindow::projectAdded);          // Works
    connect(formProjekt, &FormProjekt::projektChanged, this, &FSHKWindow::projectChanged);      // Works
    connect(formProjekt, &FormProjekt::printProject, this, &FSHKWindow::printProject);

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
    connect(formEvaluation, &FormEvaluation::formEvaluationClosed, this, &FSHKWindow::formHasClosed);
    connect(formEvaluation, &FormEvaluation::printResultList, this, &FSHKWindow::printResultList);

    //connect(formEvaluation, &FormEvaluation::recoverAll, this, &FSHKWindow::recoverAll);

    connect(ui->actionBeenden, &QAction::triggered, this, &FSHKWindow::actionCloseClicked);
    connect(ui->actionInfo, &QAction::triggered, this, &FSHKWindow::actionInfoClicked);
    connect(ui->actionCompany, &QAction::triggered, this, &FSHKWindow::actionCompanyClicked);
    connect(ui->actionApprentice, &QAction::triggered, this, &FSHKWindow::actionApprenticeClicked);
    connect(ui->actionProjects, &QAction::triggered, this, &FSHKWindow::actionProjectClicked);
    connect(ui->actionSkill, &QAction::triggered, this, &FSHKWindow::actionSkillClicked);
    connect(ui->actionAllocate, &QAction::triggered, this, &FSHKWindow::actionAllocateClicked);
    connect(ui->actionEvaluation, &QAction::triggered, this, &FSHKWindow::actionEvaluationClicked);
    connect(ui->actionSettings, &QAction::triggered, this, &FSHKWindow::actionSettingsClicked);

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
    DialogInfo *infoDlg = new DialogInfo(qApp->applicationName(), "Version " + qApp->applicationVersion(), this);
    infoDlg->exec();
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

    // Check if all apprentice skills exist
//    if(getSkillDataIncomplete().isEmpty())
//        formEvaluation->setRecoverButtonEnable(false, QStringList());
//    else
    //        formEvaluation->setRecoverButtonEnable(true, getSkillDataIncomplete());
}

void FSHKWindow::actionSettingsClicked()
{
    int result = dialogAppSettings->exec();
    if(result == QMessageBox::Accepted)
    {
        auto map = dialogAppSettings->verfy();

        if(map.value("path").toString() != appSettingsMap.value("path").toString())
        {
            int ok = QMessageBox::question(this, tr("Programmverzeichnis"), tr("Das Verzeichnis des Programmes hat"
                     " sich verändert. Soll die bereits vorhandenen Daten in das neue Verzeichnis kopiert werden?"),
                     QMessageBox::Yes | QMessageBox::No);
            if(ok == QMessageBox::Yes)
            {
                if(copyDatas(appSettingsMap.value("path").toString(), map.value("path").toString() ))
                {
                    appSettingsMap.insert("path", map.value("path").toString());
                    ui->statusBar->showMessage("Daten wurden erfolgreich Kopiert", 6000);
                }
                else
                {
                    QMessageBox::information(this, tr("Kopieren"), tr("Die Daten konnten nicht ins neue"
                             " Verzeichnis kopiert werden!\nBitte überprüfen sie ihre Schreibberechtigung!"));
                }
            }
        }

        appSettingsMap.insert("orgname", map.value("orgname").toString());
        appSettingsMap.insert("checkupdate", map.value("checkupdate").toBool());

    }
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

void FSHKWindow::printApprentice(const QList<ClassLehrling> &apprenticeList)
{
//    foreach (ClassLehrling appr, apprenticeList) {
//        qDebug() << appr.surname();
//    }

    ClassPrinting *print = new ClassPrinting(this);

    if(!print->setupPrinter(QPageLayout::Landscape))
        return;

    print->setCaption(appSettingsMap.value("orgname").toString());
    print->setText( "Liste:" );
    print->setDocName("Lehrlings List");
    print->setDescription("Auszubildende:");
    print->setFilepath( FSHKWindow::filepath("Lehrlinge.dat"));
    print->setApprenticeList( apprenticeList );

    if(!print->print())
        QMessageBox::information(this, tr("Drucken"), tr("Drucken ist fehlgeschlagen!"));
}

void FSHKWindow::removeFromCompany(const QString &oldApprKey, const QString &company)
{
    ClassBetrieb comp =getCompany(company);
    comp.removeApprentice( oldApprKey );
    companyMap.insert(comp.nr(), comp);
    saveDatas("Betriebe.dat");
}

/// Signals from FormProject
///
void FSHKWindow::saveProjectMap(const QMap<QString, ClassProjekt> &pMap)
{
    projectMap = pMap;
    saveDatas("Projekte.dat");
    setupMenu();
}

void FSHKWindow::printProject(const ClassProjekt &pro)
{

    ClassPrinting *print = new ClassPrinting(this);

    if( !print->setupPrinter( QPageLayout::Portrait ) )
        return;

    print->setCaption(appSettingsMap.value("orgname").toString());
    print->setText( "Bewertung:" );
    print->setDocName(pro.name());
    print->setProject(pro);
    print->setDescription(pro.getDescrition());
    print->setFilepath( FSHKWindow::filepath("Projekte.dat"));

    print->print();

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

    // Copy project into skill, this will overwrite the old project
    foreach (ClassSkills sk, skillMap.values())
    {

        if(sk.identifier() == pro.identifier())
        {
            sk.insertProjekt(pro);
            skillMap.insert(sk.getKey(), sk);

            // Store the changed skill's
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
    // Change the base project
    ClassSkills sk = skill;
    bool projectDataChanged = false;
    foreach (ClassProjekt pro, sk.getProjektMap().values())
    {
        ClassProjekt basePro = projectMap.value(pro.getKey());
        if(basePro.isValid())
        {
            // Compare factor of project
            if(basePro.getFactor() != pro.getFactor())
            {
                basePro.setFactor( pro.getFactor() );
                projectMap.insert(basePro.getKey(), basePro);
                projectDataChanged = true;
            }
        }
    }

    if(projectDataChanged)
        saveDatas("Projekte.dat");


    // Chek if any apprentice has this skill
    bool apprenticeDataChanged = false;
    foreach (ClassLehrling azu, apprenticeMap.values())
    {
        // If apprentice has the changed skill
        if(azu.containsSkill(sk.getKey()))
        {
            // If no data has been evaluated
            if(!azu.isSkillEvaluated(sk.getKey()))
                azu.insertSkill(sk);
            else
            {
                // Check differences of projects
                ClassSkills oldSkill = azu.getSkill(sk.getKey());
                foreach (ClassProjekt newPro, sk.getProjektMap().values())
                {
                    // If the newPro in oldSkill exist
                    if(oldSkill.containsProject(newPro.getKey()))
                    {
                        ClassProjekt oldPro = oldSkill.getProjektMap().value(newPro.getKey());
                        oldPro.setFactor( newPro.getFactor());
                        oldSkill.insertProjekt( oldPro );
                    }
                    else
                        oldSkill.insertProjekt(newPro);

                }

                // Set criteria of evaluation
                oldSkill.setEvaluationType( sk.getEvaluationIndex() );

                // Set factor of skill
                oldSkill.setWert( sk.getWert() );

                azu.insertSkill( oldSkill );
            }

            apprenticeMap.insert(azu.getKey(), azu);
            apprenticeDataChanged = true;
        }
    }

    if(apprenticeDataChanged)
        saveDatas("Lehrlinge.dat");
}

void FSHKWindow::recoverAll()
{
    bool projectDataChanged = false;
    bool skillDataChanged = false;

    QStringList dataKeys;

    foreach (ClassLehrling azu, apprenticeMap.values())
    {
        foreach (ClassSkills sk, azu.getSkillMap().values())
        {
            foreach (ClassProjekt pro, sk.getProjektMap().values())
            {
                ClassProjekt p = projectMap.value(pro.getKey());
                if(!p.isValid())
                {
                    pro.clearValues();
                    projectMap.insert(pro.getKey(), pro);
                    projectDataChanged = true;
                    dataKeys << pro.getKey();
                }
            }

            ClassSkills k = skillMap.value(sk.getKey());
            if(!k.isValid())
            {
                skillMap.insert(sk.getKey(), sk);
                skillDataChanged = true;
                dataKeys << sk.getKey();
            }
        }
    }

    if(projectDataChanged)
        saveDatas("Projekte.dat");

    if(skillDataChanged)
        saveDatas("Pruefungen.dat");

    if(!dataKeys.isEmpty())
    {
        QString title = "Projekte und Prüfungen";
        QString message = "Die unten aufgeführten Projekte und Prüfungen wurden erfogreich wieder hergestellt!";

        DialogApprenticeList *dlg = new  DialogApprenticeList(title, message, dataKeys ,this);
        dlg->exec();
    }
}

void FSHKWindow::printResultList(const QMap<int, QVariant> &map)
{

    QMapIterator<int, QVariant> it(map);
    while (it.hasNext()) {
        it.next();
        qDebug() << "key" << it.key();
        qDebug() << "value" << it.value();

    }
    ClassPrinting *print = new ClassPrinting(this);

    if(!print->setupPrinter(QPageLayout::Landscape))
        return;

    print->setCaption(appSettingsMap.value("orgname").toString());
    print->setText( "Ergebnis-Liste: ");
    print->setDocName("Lehrlings Ergebnisliste");
    print->setDescription("Auszubildende:");
    print->setResultMap(map);

    if(!print->print())
        QMessageBox::information(this, tr("Drucken"), tr("Drucken ist fehlgeschlagen!"));
    qDebug() << "printing result list";
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

QStringList FSHKWindow::getSkillDataIncomplete()
{
    QStringList keyList;
    foreach(ClassLehrling azu, apprenticeMap.values())
    {
        foreach (ClassSkills sk, azu.getSkillMap().values())
        {
            ClassSkills p = skillMap.value( sk.getKey());
            if(!p.isValid())
            {
                if(!keyList.contains(sk.getKey()))
                    keyList << sk.getKey();
            }
        }
    }
    return keyList;
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
            if(pro.isValid())
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
        while (it.hasNext())
        {
            it.next();
            ClassProjekt pro = it.value();
            if(pro.isValid())
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

bool FSHKWindow::copyDatas(const QString &oldPath, const QString &newPath)
{
    QDir::setCurrent( newPath );
    bool status;

    status = saveDatas("Betriebe.dat");
    status = saveDatas("Lehrlinge.dat");
    status = saveDatas("Projekte.dat");
    status = saveDatas("Pruefungen.dat");

    return status;
}

void FSHKWindow::setFontAttribute(int size, bool bold, QColor col, QPainter *p)
{
    QFont font = p->font();
    QPen pen = p->pen();

    font.setPointSize(size);
    font.setBold(bold);
    pen.setColor(col);

    p->setFont(font);
    p->setPen(pen);
}

qreal FSHKWindow::textWidth(const QString &text, const QPainter &p)
{
    QFontMetricsF fm(p.font());
    return fm.width(text);
}

QStringList FSHKWindow::splitText(const QString &text, qreal maxLength, const QPainter &p)
{
    QStringList list = text.split(" ");

    QStringList textList;
    QString string = "";


    foreach (QString s, list)
    {
        if( textWidth(string+s+" ", p) < maxLength)
        {
            string.append(s);
            string.append(" ");

            /*if(list.last() == s)
                textList << string*/;
        }
        else
        {
            textList << string;
            string.clear();
            string.append(s);
            string.append(" ");
        }
    }

    textList << string;


    return textList;
}

qreal FSHKWindow::lineLength(qreal x1, qreal x2)
{
    QPointF p1; p1.setX(x1); p1.setY(0);
    QPointF p2; p2.setX(x2); p2.setY(0);
    QLineF l(p1, p2);

    return l.length();
}

qreal FSHKWindow::pixel(qreal millimeter, const QPrinter &p)
{
    qreal fac = p.width() / p.widthMM();
    return fac * millimeter;
}


qreal FSHKWindow::millimeter(qreal pix, const QPrinter &p)
{
    qreal fac = p.widthMM() / p.width();
    return pix * fac;
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
                // If skill is already evaluated
                if(azu.isSkillEvaluated(skillKeyList.at(i)))
                    azuSkillMap.insert(azu.getKey(), "evaluatedSkill");
                else
                {
                    ClassSkills sk = azu.getSkill( skillKeyList.at(i));
                    if(order == "remove")
                        sk.removeProject(proKey);

                    if(order == "add"){
                        ClassProjekt pro = projectMap.value(proKey);
                        sk.insertProjekt(pro);
                    }

                    azu.insertSkill(sk);
                    apprenticeMap.insert(azu.getKey(), azu);
                }
            }
        }
    }

    saveDatas("Lehrlinge.dat");


    if(azuSkillMap.isEmpty())
        return;

    QStringList azuKeyList;
    azuKeyList << azuSkillMap.keys("evaluatedSkill");

    if(!azuKeyList.isEmpty())
    {
        QString title = "Geänderte Prüfunge";
        QString message = "Bei den unten aufgeführten Auszubildenden wurde die Prüfung bereits "
                          "ganz oder teils ausgewertet.\nDurch die Änderung können ausgewertete Daten verloren gehen!";

        DialogApprenticeList *dlg = new  DialogApprenticeList( title, message, azuKeyList ,this);

        if(dlg->exec() == QDialog::Rejected)
            return;
    }


    foreach (QString key, azuKeyList)
    {
        ClassLehrling appr = apprenticeMap.value(key);
        for(int i = 0; i < skillKeyList.size(); i++)
        {

            ClassSkills sk = appr.getSkill( skillKeyList.at(i));

            if(order == "remove")
                sk.removeProject(proKey);

             // Check what was changed if project exist
             if(order == "add" && sk.containsProject(proKey)  )
             {
                 ClassProjekt newPro = projectMap.value(proKey);
                 ClassProjekt oldPro = sk.getProjektMap().value(proKey);

                 oldPro.setEvaluated( false );
                 oldPro.setFactor( newPro.getFactor() );
                 oldPro.setMaxPoints( newPro.maxPoints() );

                 // Compare questions
                 if(oldPro.questionMap().size() != newPro.questionMap().size())
                     oldPro.setQuestionMap( newPro.questionMap() );
                 else
                 {
                     int index = 0;
                     QMap<int, ClassFrage> oqMap;
                     foreach (ClassFrage nq, newPro.questionMap().values())
                     {
                        ClassFrage oq = oldPro.questionMap().value(index);
                        oq.setQuestion(nq.question());
                        oq.setIdentifier( nq.identifier() );
                        oq.setMaxPoints( nq.maxPoints() );
                        oqMap.insert(index, oq);
                        index++;
                     }

                     oldPro.setQuestionMap( oqMap );
                 }

                 sk.insertProjekt( oldPro );
                 appr.insertSkill(sk);
             }
             else // if project not exist
             {
                sk.insertProjekt( projectMap.value(proKey) );
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

    if(projectMap.isEmpty())
        ui->actionSkill->setEnabled(false);
    else
        ui->actionSkill->setEnabled(true);

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

    appSettingsMap.insert("orgname", settings.value("orgname","").toString());
    appSettingsMap.insert("path", settings.value("path", QDir::currentPath() ).toString());
    QDir::setCurrent( appSettingsMap.value("path").toString());

    appSettingsMap.insert("checkupdate", settings.value("checkupdate",false).toBool() );
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

    settings.setValue("orgname", appSettingsMap.value("orgname", "").toString());
    settings.setValue("path", appSettingsMap.value("path", QDir::currentPath() ).toString());
    settings.setValue("checkupdate", appSettingsMap.value("checkupdate").toBool());
}
