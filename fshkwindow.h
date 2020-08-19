#ifndef FSHKWINDOW_H
#define FSHKWINDOW_H

#include <QMainWindow>

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


private:
    Ui::FSHKWindow *ui;


    void readSettings();
    void writeSettings();
};

#endif // FSHKWINDOW_H
