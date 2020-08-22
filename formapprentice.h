#ifndef FORMAPPRENTICE_H
#define FORMAPPRENTICE_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QMap>
#include <QStandardItemModel>
#include <QModelIndex>

#include <classlehrling.h>


namespace Ui {
class FormLehrling;
}

class FormLehrling : public QWidget
{
    Q_OBJECT

public:
    explicit FormLehrling(QWidget *parent = nullptr);
    ~FormLehrling();


    QMap<QString, ClassLehrling> apprenticeMap() const;
    void setApprenticeMap(const QMap<QString, ClassLehrling> &apprenticeMap);

signals:
    void apprenticeFormClosed();
    void saveApprenticeMap(const QMap<QString, ClassLehrling> &aMap);

private slots:

    void closeButtonClicked();
    void createButtonClicked();
    void deleteButtonClicked();
    void deleteSkillButtonClicked();
    void changeButtonClicked();
    void saveButtonClicked();

    void klasseBoxTextChanged(const QString &text);
    void apprenticeTableClicked(QTableWidgetItem *item);

private:
    Ui::FormLehrling *ui;

    bool changeData;
    ClassLehrling seletedApprentice;
    QMap<QString, ClassLehrling> m_apprenticeMap;
    QMap<QString, QVariant> klasseMap;

    void sortApprenticeTable();
    QMap<QString, ClassLehrling> getApprenticeMap(int year);
    void updateApprenticeTable(QTableWidget *widget, const QMap<QString, ClassLehrling> &aMap);

    void setFormReadOnly(bool status);
    void setApprenticeToForm(const ClassLehrling &appr);
    ClassLehrling readFromForm();
    void setFormTextColor(QColor color);
    void clearForm();
    void setupKlasseMap();

};

#endif // FORMLEHRLING_H