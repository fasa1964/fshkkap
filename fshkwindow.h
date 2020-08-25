#ifndef FSHKWINDOW_H
#define FSHKWINDOW_H

#include <QMainWindow>
#include <QLabel>


#include <appwidget.h>

#include <classbetrieb.h>
#include <formcompany.h>

#include <classlehrling.h>
#include <formapprentice.h>

#include <classprojekt.h>
#include <formproject.h>

#include <classskills.h>
#include <formskills.h>


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

    // Signal from Forms***
    void formHasClosed();

    // Signals from FormsCompany
    void saveCompanyMap(const QMap<int, ClassBetrieb> &cMap);
    void companyRemoved(const QString &company);
    void apprenticeRemoved(const QString &company, const QString &apprenticeKey);

    // Signals from FormApprentice
    void saveApprenticeMap(const QMap<QString, ClassLehrling> &aMap);
    void apprenticeWithoutCompany(const ClassLehrling &appr);
    void apprenticeAssociatedCompany(const QString &company, const QString &apprenticeKey);

    // Signals from FormProject
    void saveProjectMap(const QMap<QString, ClassProjekt> &pMap);

    // Signals from FormSkills
    void saveSkillMap(const QMap<QString, ClassSkills> &sMap);

private:
    Ui::FSHKWindow *ui;

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

    QDateTime lastFileModified(const QString &filename);

    void readDatas(const QString &filename);
    bool saveDatas(const QString &filename);


    void readSettings();
    void writeSettings();
};

#endif // FSHKWINDOW_H
