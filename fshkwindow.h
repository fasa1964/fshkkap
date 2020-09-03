#ifndef FSHKWINDOW_H
#define FSHKWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>

#include <appwidget.h>

#include <classbetrieb.h>
#include <formcompany.h>

#include <classlehrling.h>
#include <formapprentice.h>

#include <classprojekt.h>
#include <formproject.h>

#include <classskills.h>
#include <formskills.h>

#include <formallocate.h>
#include <formevaluation.h>

#include <dialogapprenticelist.h>

namespace Ui {
class FSHKWindow;
}

class FSHKWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FSHKWindow(QWidget *parent = 0);
    ~FSHKWindow();

private slots:
    void actionCloseClicked();
    void actionInfoClicked();

    void actionCompanyClicked();
    void actionApprenticeClicked();
    void actionProjectClicked();
    void actionSkillClicked();
    void actionAllocateClicked();
    void actionEvaluationClicked();

    // Signal from Forms***
    void formHasClosed();

    // Signals from FormsCompany
    void saveCompanyMap(const QMap<int, ClassBetrieb> &cMap);
    void companyRemoved(const QString &company);
    void apprenticeRemoved(const QString &company, const QString &apprenticeKey);

    // Signals from FormApprentice
    void saveApprenticeMap(const QMap<QString, ClassLehrling> &aMap);
    void apprenticeWithoutCompany(const QString &apprKey);
    void apprenticeAssociatedCompany(const QString &company, const QString &apprenticeKey);

    // Signals from FormProject
    void saveProjectMap(const QMap<QString, ClassProjekt> &pMap);
    void projectRemoved(const QString &proKey);
    void projectAdded(const ClassProjekt &pro);
    void projectChanged(const ClassProjekt &pro);

    // Signals from FormSkills
    void saveSkillMap(const QMap<QString, ClassSkills> &sMap);
    void removeProjects(const QMap<QString, ClassProjekt> &proMap);
    void skillChanged(const ClassSkills &skill);

private:
    Ui::FSHKWindow *ui;
    QPushButton *dateButton;

    bool skillSameIdentifier(const QString &proIdent);
    QStringList addProjectToSkill(const ClassProjekt &pro);
    QStringList removeProjectFromSkill(const ClassProjekt &pro);
    void addProjectToApprentice(const QString &apprKey, const QString &skillKey, const ClassProjekt &pro );

    //bool skillListContainsProject(const QString &proKey);
    QMap<QString, QVariant> getApprenticeKeyMap(const QString &skillKey);


//    QStringList getApprenticeList(const ClassSkills &skill);
//    void insertProjectToSkill(const ClassProjekt &pro);
//    void addProjectToSkill(const ClassProjekt &pro);
//    void insertProjectToApprentice(const ClassProjekt &pro);
//    void addProjectToApprentice(const ClassProjekt &pro);

    AppWidget *appwidget;
    void setApplicationLabel();

    FormBetrieb *formCompany;
    QMap<int, ClassBetrieb> companyMap;
    ClassBetrieb getCompany(const QString &name);

    FormLehrling *formApprentice;
    QMap<QString,ClassLehrling> apprenticeMap;

    FormProjekt *formProjekt;
    QMap<QString, ClassProjekt> projectMap;

    FormSkills *formSkill;
    QMap<QString, ClassSkills> skillMap;

    FormAllocate *formAllocate;
    FormEvaluation *formEvaluation;


    QDateTime lastFileModified(const QString &filename);

    void updateCompanyData();
    QStringList companyKeyList();

    void updateApprenticeData();
    bool apprenticeExist(const QString &key);

    void updateProjektData();
    QStringList projectKeyList(const QString &identifier);

    void setupMenu();
    void readDatas(const QString &filename);
    bool saveDatas(const QString &filename);

    // Values for app settings
    QMap<QString, QVariant> appSettingsMap;


    void readSettings();
    void writeSettings();
};

#endif // FSHKWINDOW_H
