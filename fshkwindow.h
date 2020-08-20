#ifndef FSHKWINDOW_H
#define FSHKWINDOW_H

#include <QMainWindow>
#include <QLabel>


#include <appwidget.h>


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


private:
    Ui::FSHKWindow *ui;

    AppWidget *appwidget;
    void setApplicationLabel();

    void readSettings();
    void writeSettings();
};

#endif // FSHKWINDOW_H
