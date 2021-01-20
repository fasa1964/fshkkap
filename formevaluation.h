#ifndef FORMEVALUATION_H
#define FORMEVALUATION_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QTreeWidgetItem>

#include <classlehrling.h>
#include <classprojekt.h>
#include <classskills.h>
#include <classfrage.h>

#include <formapprenticeresultlist.h>

namespace Ui {
class FormEvaluation;
}

class FormEvaluation : public QWidget
{
    Q_OBJECT

public:
    explicit FormEvaluation(QWidget *parent = nullptr);
    ~FormEvaluation();

     void updateSortBox();

     QMap<QString, ClassLehrling> getApprenticeMap() const;
     void setApprenticeMap(const QMap<QString, ClassLehrling> &apprenticeMap);

//     void setRecoverButtonEnable(bool status, const QStringList &skillKeys);

signals:
     void saveApprenticeMap(QMap<QString, ClassLehrling> apprMap);
     void formEvaluationClosed();
     void printResultList(const QMap<int, QVariant> &map);
//     void recoverAll();

private slots:
     void closeButtonClicked();
     void saveButtonClicked();
     void listButtonClicked();
     void printButtonClicked(const QMap<int, QVariant> &map);

     void azubiSortBoxChanged(const QString &text);
     void azubiListBoxChanged(const QString &text);
     void azubiSkillBoxChanged(const QString &text);
     void azubiProjectBoxChanged(const QString &text);
     void evaluatedCheckBoxChanged(int status);


     void questionTableCellChanged(int row, int column);
     void resultTableItemClicked(QTreeWidgetItem *item, int);

//     void recoverButtonClicked();
//     void sortBoxTextChanged(const QString &text);
//     void azubiBoxTextChanged(const QString &text);
//     void skillBoxTextChanged(const QString &text);
//     void projectBoxTextChanged(const QString &text);

//     void evaluatedCheckBoxChanged(int status);

//     void resultTableItemClicked(QTreeWidgetItem *item, int column);
//     void factorBoxChanged(double value);

private:
    Ui::FormEvaluation *ui;

    bool dirty;
    ClassLehrling selectedApprentice;
    ClassSkills selectedSkill;
    ClassProjekt selectedProjekt;
//    QMap<QString, ClassSkills> selectedSkillMap;
//    QMap<QString, ClassProjekt> selectedProjectMap;

    QMap<QString, ClassLehrling> m_apprenticeMap;

    //QMap<QString, QString> apprYearMap;
    void setupApprYearBox();    // Sort the apprentice by years
    QStringList getApprenticeKeyList(int year);

    void setupQuestionTable(const ClassProjekt &pro);

    // Test result item map
    double apprenticePercent(const ClassLehrling &);
    double skillPercent(ClassSkills skill);
    double projectPercent(const ClassProjekt &pro);
    int projectPoints(const ClassProjekt &pro);


    double totalProjectPercent(const ClassSkills &skill);

    void storeValues();
    void updateResultTreeWidget(const ClassLehrling &appr);

    FormApprenticeResultList *apprenticeTableList;
//    void setupProjectValue();
//    void setupQuestionTable(ClassProjekt pro);
//    void updateSkillBox(const ClassLehrling &azu);
//    QMap<QString, ClassLehrling> apprenticeship(int year);
//    void setTextColor(QWidget *widget, QColor color);

//    // Test
//    void setupResultWidget(const ClassLehrling &azu);
//    void setupIdentifier(const ClassLehrling &azu);


//    QStringList questionsIdentifierList(const ClassLehrling &azu);

//    QList<ClassFrage> getQuestions(const ClassProjekt &pro, const QString &key);
//    double getResultIdentifier(const QList<ClassFrage> questList);

//    bool isSkillFactorValid();
//    double getSkillFactor(const ClassSkills &skill);
//    double getProjectPercent(const ClassProjekt &pro);
//    int getProjectMaxPoint(const ClassProjekt &pro);
//    int getProjectPoints(const ClassProjekt &pro);

//    /// !brief Returns true if skill key
//    bool isSkillKey(const QString &text);
    bool isDigit(const QString &text);
//    void setTextColor(QWidget *widget, QColor color);
    void setTextColor(QWidget *widget, double percent);
    void setItemColor(QTreeWidgetItem *item, double percent);
    void sendMessage(const QString &text, const QPixmap &pixmap, bool show);
};


#endif // FORMEVALUATION_H
