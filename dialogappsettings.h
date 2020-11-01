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
    explicit DialogAppSettings(const QMap<QString, QVariant> &map, QWidget *parent = 0);
    ~DialogAppSettings();

    QMap<QString, QVariant> verfy();

private slots:
    void pathButtonClicked();

private:
    Ui::DialogAppSettings *ui;
};

#endif // DIALOGAPPSETTINGS_H
