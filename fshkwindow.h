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

private:
    Ui::FSHKWindow *ui;
};

#endif // FSHKWINDOW_H
