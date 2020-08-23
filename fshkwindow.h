#ifndef FSHKWINDOW_H
#define FSHKWINDOW_H

#include <QMainWindow>
#include <QLabel>


#include <appwidget.h>

#include <classbetrieb.h>
#include <formcompany.h>

#include <classlehrling.h>
#include <formapprentice.h>


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

    // Signal from Forms
    void formHasClosed();

    // Signals from FormsCompany
    void saveCompanyMap(const QMap<int, ClassBetrieb> &cMap);

    // Signals from FormApprentice
    void saveApprenticeMap(const QMap<QString, ClassLehrling> &aMap);
    void apprenticeWithoutCompany(const ClassLehrling &appr);
    void apprenticeAssociatedCompany(const QString &company, const QString &apprenticeKey);

private:
    Ui::FSHKWindow *ui;

    AppWidget *appwidget;
    void setApplicationLabel();

    FormBetrieb *formCompany;
    QMap<int, ClassBetrieb> companyMap;
    ClassBetrieb getCompany(const QString &name);

    FormLehrling *formApprentice;
    QMap<QString,ClassLehrling> apprenticeMap;

    QDateTime lastFileModified(const QString &filename);

    void readDatas(const QString &filename);
    bool saveDatas(const QString &filename);


    void readSettings();
    void writeSettings();
};

#endif // FSHKWINDOW_H
