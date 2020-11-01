#ifndef DIALOGAPPSETTINGS_H
#define DIALOGAPPSETTINGS_H

#include <QDialog>

namespace Ui {
class DialogAppSettings;
}

class DialogAppSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAppSettings(QWidget *parent = 0);
    ~DialogAppSettings();

private:
    Ui::DialogAppSettings *ui;
};

#endif // DIALOGAPPSETTINGS_H
