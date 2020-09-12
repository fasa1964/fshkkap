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
    void skillChanged(const ClassSkills &skill);
    void removeProjects(const QMap<QString, ClassProjekt> &proMap);


private slots:
    void closeButtonClicked();
    void createButtonClicked();
    void changeButtonClicked();
    void saveButtonClicked();
    void deleteButtonClicked();
    void deleteSkillProjektButtonClicked();

    void importProjectButtonClicked();
    void criteriaBoxChanged(int index);

    void skillTableItemClicked(QTableWidgetItem *item);
    void projektTableItemClicked(QTableWidgetItem *);
    void skillProjektTableItemClicked(QTableWidgetItem *);
    void skillProjektTableCellClicked(int row, int column);

    void sortProjectBoxTextChanged(const QString &text);

private:
    Ui::FormSkills *ui;

    bool changeSkill;
    bool createSkill;
    ClassSkills selectedSkill;

    QMap<QString, ClassSkills> m_skillMap;
    ClassSkills readFromForm();


    QMap<QString, ClassProjekt> m_projektMap;


    QStringList getProjectKennung(const QMap<QString, ClassProjekt> &pMap);
    QMap<QString, ClassProjekt> getSkillProjectMap();
    QMap<QString, ClassProjekt> getSelectedProjects();
    bool isItemChecked(QTableWidget *widget);

    void setupSkillTable(const QMap<QString, ClassSkills> &sMap);
    void setupSkillProjectTable(const QMap<QString, ClassProjekt> &pMap);
    void setupProjektTable(const QMap<QString, ClassProjekt> &pMap, Qt::CheckState state);
    void setSkillToForm(const ClassSkills &skill);
    void setFormTextColor(QColor color);
    void setFormReadOnly(bool status);
    void clearForm();

};

#endif // FORMSKILLS_H
