#ifndef FORMAPPRENTICE_H
#define FORMAPPRENTICE_H

#include <QWidget>
#include <QTableWidgetItem>
#include <QMap>
#include <QStandardItemModel>
#include <QModelIndex>

#include <classlehrling.h>
#include <classbetrieb.h>


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

    QMap<int, ClassBetrieb> getCompanyMap() const;
    void setCompanyMap(const QMap<int, ClassBetrieb> &companyMap);

    void setLastModified(const QDateTime &date);

signals:
    void apprenticeFormClosed();
    void saveApprenticeMap(const QMap<QString, ClassLehrling> &aMap);
    void apprenticeWithoutCompany(const QString &apprKey);
    void apprenticeHasCompany(const QString &company, const QString &apprenticeKey);
    void printApprentice(const QList<ClassLehrling> &apprList);
    void removeFromCompany(const QString &oldApprKey, const QString &company);

private slots:

    void closeButtonClicked();
    void createButtonClicked();
    void deleteButtonClicked();
    void deleteSkillButtonClicked();
    void changeButtonClicked();
    void saveButtonClicked();
    void printButtonClicked();
    void companyViewButtonClicked();

    void klasseBoxTextChanged(const QString &text);
    void apprenticeTableClicked(QTableWidgetItem *item);

private:
    Ui::FormLehrling *ui;

    bool changeData;
    ClassLehrling seletedApprentice;
    QMap<QString, ClassLehrling> m_apprenticeMap;
    QMap<QString, QVariant> klasseMap;

    /// !brief for the CompanyViewList
    /// to pick a company
    QMap<int, ClassBetrieb> m_companyMap;
    bool companyExist(const QString &name);

    /// !brief Returns the trainy year
    int getEducationYear(const ClassLehrling &appr);
    bool examNumberExist(int educationYear, int examNumber);

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
