#ifndef APPWIDGET_H
#define APPWIDGET_H

#include <QWidget>

namespace Ui {
class AppWidget;
}

class AppWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AppWidget(const QString &NAME, const QString &VERSION, QWidget *parent = 0);
    ~AppWidget();

private:
    Ui::AppWidget *ui;
};

#endif // APPWIDGET_H
