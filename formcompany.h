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

signals:
    void companyFormClosed();
    void saveCompanyMap(const QMap<int, ClassBetrieb> &bMap);
    void apprenticeRemoved(const QList<ClassLehrling> &azuList, const ClassBetrieb &company );

private slots:
    void closeButtonClicked();
    void createButtonClicked();
    void deleteButtonClicked();
    void changeButtonClicked();
    void saveButtonClicked();

    void sortBoxTextChanged(const QString &text);
    void companyTableItemClicked(QTableWidgetItem *item);

private:
    Ui::FormBetrieb *ui;

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
