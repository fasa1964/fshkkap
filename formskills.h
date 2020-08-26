#ifndef FORMSKILLS_H
#define FORMSKILLS_H

#include <QWidget>
#include <QTableWidgetItem>

#include <classfrage.h>
#include <classprojekt.h>
#include <classskills.h>

namespace Ui {
class FormSkills;
}

class FormSkills : public QWidget
{
    Q_OBJECT

public:
    explicit FormSkills(QWidget *parent = nullptr);
    ~FormSkills();

    QMap<QString, ClassSkills> skillMap() const;
    void setSkillMap(const QMap<QString, ClassSkills> &skillMap);

    QMap<QString, ClassProjekt> projektMap() const;
    void setProjektMap(const QMap<QString, ClassProjekt> &projektMap);

signals:
    void formSkillClosed();
    void saveSkillsMap(const QMap<QString, ClassSkills> &skillMap);
    //    void skillChanged(ClassSkills skill);
//    void skillProjektChanged(ClassProjekt pro);

private slots:
    void closeButtonClicked();
    void createButtonClicked();
    void saveButtonClicked();
//    void deleteSkillProjektButtonClicked();

//    void deleteButtonClicked();
//    void changeButtonClicked();
//
    void importProjectButtonClicked();
//    void dragDropButtonClicked();


//    void sortKennungBoxTextChanged(const QString &text);
//    void projektTableItemClicked(QTableWidgetItem *);

//    void skillTableItemClicked(QTableWidgetItem *item);
    void projektTableItemClicked(QTableWidgetItem *);
    void skillProjektTableItemClicked(QTableWidgetItem *);
//    void skillProjektTableCellClicked(int row, int column);


//    void kennungBoxTextChanged(const QString &text);
    void sortProjectBoxTextChanged(const QString &text);

private:
    Ui::FormSkills *ui;

    bool changeSkill;
    bool createSkill;
//    bool projectFactorChanged;

    QMap<QString, ClassSkills> m_skillMap;
    ClassSkills readFromForm();


//    ClassSkills selectedSkill;
//    void setSkillToForm(const ClassSkills &skill);
//    void setSkillProjektToForm(const QMap<QString, ClassProjekt> &proMap);

//    QMap<QString, ClassProjekt> getSkillProjektMap();
//    double totalProjectFactor(const ClassSkills &skill);

    QMap<QString, ClassProjekt> m_projektMap;
//    void setupKennungBox(const QMap<QString, ClassProjekt> &proMap);
//    void setupProjektTable(const QMap<QString, ClassProjekt> &proMap, Qt::CheckState state);
//    void setupProjektIconTable(const QMap<QString, ClassProjekt> &proMap, Qt::CheckState state);
//    double getSkillProjectFactor();

    QStringList getProjectKennung(const QMap<QString, ClassProjekt> &pMap);
    QMap<QString, ClassProjekt> getSkillProjectMap();
    QMap<QString, ClassProjekt> getSelectedProjects();
    bool isItemChecked(QTableWidget *widget);



    void setupSkillTable(const QMap<QString, ClassSkills> &sMap);
    void setupSkillProjectTable(const QMap<QString, ClassProjekt> &pMap);
    void setupProjektTable(const QMap<QString, ClassProjekt> &pMap, Qt::CheckState state);
    void setFormTextColor(QColor color);
    void setFormReadOnly(bool status);
    void clearForm();

};

#endif // FORMSKILLS_H
