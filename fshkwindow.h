#ifndef FSHKWINDOW_H
#define FSHKWINDOW_H

#include <QMainWindow>
#include <QLabel>


#include <appwidget.h>

#include <classbetrieb.h>
#include <formcompany.h>

#include<classlehrling.h>


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

    // Signal from FormsCompany
    void saveCompanyMap(const QMap<int, ClassBetrieb> &cMap);
    void apprenticeRemoved(const QList<ClassLehrling> &azuList, const ClassBetrieb &company);

private:
    Ui::FSHKWindow *ui;

    AppWidget *appwidget;
    void setApplicationLabel();

    FormBetrieb *formCompany;
    QMap<int, ClassBetrieb> companyMap;

    QMap<QString,ClassLehrling> apprenticeMap;

    void readDatas(const QString &filename);
    bool saveDatas(const QString &filename);


    void readSettings();
    void writeSettings();
};

#endif // FSHKWINDOW_H
