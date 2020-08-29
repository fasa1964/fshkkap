#ifndef FORMALLOCATE_H
#define FORMALLOCATE_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QListWidgetItem>

#include <classlehrling.h>
#include <classskills.h>

namespace Ui {
class FormAllocate;
}

class FormAllocate : public QWidget
{
    Q_OBJECT

public:
    explicit FormAllocate(QWidget *parent = 0);
    ~FormAllocate();

    QMap<QString, ClassSkills> skillMap() const;
    void setSkillMap(const QMap<QString, ClassSkills> &skillMap);

    QMap<QString, ClassLehrling> apprenticeMap() const;
    void setApprenticeMap(const QMap<QString, ClassLehrling> &apprenticeMap);

signals:
    void formAllocateClosed();
    void saveApprenticeMap(const QMap<QString, ClassLehrling> &aMap);

private slots:
    void closeButtonClicked();
    void saveButtonClicked();
    void importButtonClicked();
    void skillTableItemClicked(QListWidgetItem *item);


private:
    Ui::FormAllocate *ui;

    ClassSkills selectedSkill;

    QMap<QString, ClassSkills> m_skillMap;
    void setupSkillTable(const QMap<QString, ClassSkills> &sMap);

    QMap<QString, ClassLehrling> m_apprenticeMap;
    void sortApprenticeTable();
    QMap<QString, ClassLehrling> getApprenticeMap(int year);
    void updateApprenticeTable(QTableWidget *widget, Qt::CheckState state, const QMap<QString, ClassLehrling> &aMap);

    QMap<QString, ClassLehrling> getSelectedApprentice(QTableWidget *widget);

//    QStringList getIdientifierList();
//    QStringList getSkillNameList(const QMap<QString, ClassSkills> &sMap);
};

#endif // FORMALLOCATE_H
