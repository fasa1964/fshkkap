#ifndef FORMAPPRENTICERESULTLIST_H
#define FORMAPPRENTICERESULTLIST_H

#include <QWidget>
#include <QTableWidgetItem>

#include <classlehrling.h>
#include <classskills.h>

namespace Ui {
class FormApprenticeResultList;
}

class FormApprenticeResultList : public QWidget
{
    Q_OBJECT

public:
    explicit FormApprenticeResultList(QWidget *parent = 0);
    ~FormApprenticeResultList();

    QList<ClassLehrling> apprenticeList() const;
    void setapprenticeList(const QList<ClassLehrling> &apprenticeList);

private:
    Ui::FormApprenticeResultList *ui;

    QList<ClassLehrling> m_apprenticeList;
    void updateTableWidget();

    void setupColor(QTableWidgetItem *item, qreal percent);

    QStringList skillNames(const QList<ClassLehrling> &list);
    double skillPercent( ClassSkills skill);
    double projectPercent(const ClassProjekt &pro);


};

#endif // FORMAPPRENTICERESULTLIST_H
