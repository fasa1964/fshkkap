#ifndef FORMAPPRENTICERESULTLIST_H
#define FORMAPPRENTICERESULTLIST_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QMap>

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


    QString caption() const;
    void setCaption(const QString &caption);

signals:
    void printResultList(const QMap<int, QVariant> &map);

private slots:
    void printButtonClicked();

private:
    Ui::FormApprenticeResultList *ui;

    QString m_caption;
    QList<ClassLehrling> m_apprenticeList;
    void updateTableWidget();

    void setupColor(QTableWidgetItem *item, qreal percent);

    QStringList skillNames(const QList<ClassLehrling> &list);
    double skillPercent( ClassSkills skill);
    double projectPercent(const ClassProjekt &pro);

    QMap<int, QVariant> resultMap;

};

#endif // FORMAPPRENTICERESULTLIST_H
