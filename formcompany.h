#ifndef FORMBETRIEB_H
#define FORMBETRIEB_H

#include <QWidget>
#include <QMap>
#include <QTableWidgetItem>

#include <classbetrieb.h>

namespace Ui {
class FormBetrieb;
}

class FormBetrieb : public QWidget
{
    Q_OBJECT

public:
    explicit FormBetrieb(QWidget *parent = nullptr);
    ~FormBetrieb();

    QMap<int, ClassBetrieb> companyMap() const;
    void setCompanyMap(const QMap<int, ClassBetrieb> &companyMap);

    void setLastModified(const QDateTime &date);

signals:
    void companyFormClosed();
    void saveCompanyMap(const QMap<int, ClassBetrieb> &bMap);
    void companyRemoved(const QString &company);
    void apprenticeRemoved(const QString &company, const QString &apprenticeKey);

private slots:
    void closeButtonClicked();
    void createButtonClicked();
    void deleteButtonClicked();
    void changeButtonClicked();
    void saveButtonClicked();
    void deleteApprenticeButtonClicked();

    void sortBoxTextChanged(const QString &text);
    void companyTableItemClicked(QTableWidgetItem *item);
    void apprenticeTableItemClicked(QTableWidgetItem *item);

private:
    Ui::FormBetrieb *ui;

    bool dataChanged;

    ClassLehrling selectedApprentice;
    ClassBetrieb selectedCompany;
    QMap<int, ClassBetrieb> m_companyMap;

    /// !brief when delete a company
    /// a nr will be missing. This function
    /// wille discover the missing nr.
    int getNextKey();

    void updateCompanyTable(const QMap<int, ClassBetrieb> bMap);
    void updateApprenticeTable(const QMap<QString, ClassLehrling> aMap);

    ClassBetrieb readFromForm();
    void setCompanyToForm(const ClassBetrieb &company);
    void setFormReadOnly(bool status);
    void setFormTextColor(QColor color);
    void clearForm();


};

#endif // FORMBETRIEB_H
