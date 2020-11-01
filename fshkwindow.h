#ifndef FSHKWINDOW_H
#define FSHKWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QtPrintSupport/QPrinter>
#include <QtMath>

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
#include <QFileInfo>

#include <dialogappsettings.h>


namespace Ui {
class FSHKWindow;
}

class FSHKWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FSHKWindow(QWidget *parent = 0);
    ~FSHKWindow();

    static int grade(double percent){

        int gradeValue = 0;
        percent = qRound(percent);
        if(percent <= 100.0 && percent >= 92.0 )
            gradeValue = 1;

        if(percent < 92.0 && percent >= 81.0 )
            gradeValue = 2;

        if(percent < 81.0 && percent >= 67.0 )
            gradeValue = 3;

        if(percent < 67.0 && percent >= 50.0 )
            gradeValue = 4;

        if(percent < 50.0 && percent >= 30.0 )
            gradeValue = 5;

        if(percent < 30.0 && percent >= 0.0 )
            gradeValue = 6;

        return gradeValue;
    }

    QString filepath(const QString &file){
        QFileInfo fi(file);
        return fi.absoluteFilePath();
    }

private slots:
    void actionCloseClicked();
    void actionInfoClicked();

    void actionCompanyClicked();
    void actionApprenticeClicked();
    void actionProjectClicked();
    void actionSkillClicked();
    void actionAllocateClicked();
    void actionEvaluationClicked();
    void actionSettingsClicked();

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
    void printApprentice(const QList<ClassLehrling> &apprenticeList);
    void removeFromCompany(const QString &oldApprKey, const QString &company);

    // Signals from FormProject
    void saveProjectMap(const QMap<QString, ClassProjekt> &pMap);
    void printProject(const ClassProjekt &pro);
    void projectRemoved(const QString &proKey);
    void projectAdded(const ClassProjekt &pro);
    void projectChanged(const ClassProjekt &pro);

    // Signals from FormSkills
    void saveSkillMap(const QMap<QString, ClassSkills> &sMap);
    void removeProjects(const QMap<QString, ClassProjekt> &proMap);
    void skillChanged(const ClassSkills &skill);

    // Signal from FormEvaluation
    void recoverAll();

private:
    Ui::FSHKWindow *ui;
    QPushButton *dateButton;

    QStringList skillKeyList(const QString &proIdentifier);
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

    // Values for app settings
    DialogAppSettings *dialogAppSettings;
    QMap<QString, QVariant> appSettingsMap;

    QStringList getSkillDataIncomplete();
    QDateTime lastFileModified(const QString &filename);

    void updateCompanyData();
    QStringList companyKeyList();

    void updateApprenticeSkillData(const QStringList &skillKeyList, const QString &proKey ,const QString &order);
    void updateApprenticeData();
    bool apprenticeExist(const QString &key);

    void updateProjektData();
    QStringList projectKeyList(const QString &identifier);

    void setupMenu();
    void readDatas(const QString &filename);
    bool saveDatas(const QString &filename);
    bool copyDatas(const QString &oldPath, const QString &newPath);

    // Printer function
    void setFontAttribute(int size, bool bold , QColor col, QPainter *p);
    qreal textWidth(const QString &text, const QPainter &p);
    QStringList splitText(const QString &text, qreal maxLength , const QPainter &p);
    qreal lineLength(qreal x1, qreal x2);
    qreal pixel(qreal millimeter, const QPrinter &p);
    qreal millimeter(qreal pix, const QPrinter &p );

    // General app settings
    void readSettings();
    void writeSettings();
};

#endif // FSHKWINDOW_H
