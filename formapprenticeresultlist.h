#ifndef FORMAPPRENTICERESULTLIST_H
#define FORMAPPRENTICERESULTLIST_H

#include <QWidget>

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

    QList<ClassLehrling> apprtenticeList() const;
    void setApprtenticeList(const QList<ClassLehrling> &apprtenticeList);

private:
    Ui::FormApprenticeResultList *ui;

    QList<ClassLehrling> m_apprtenticeList;
    void updateTableWidget();


    QStringList skillNames(const QList<ClassLehrling> &list);
    double skillPercent( ClassSkills skill);
    double projectPercent(const ClassProjekt &pro);


};

#endif // FORMAPPRENTICERESULTLIST_H
