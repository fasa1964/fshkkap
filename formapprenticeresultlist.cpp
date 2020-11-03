#include "formapprenticeresultlist.h"
#include "ui_formapprenticeresultlist.h"

#include <QBrush>
#include <QDebug>

FormApprenticeResultList::FormApprenticeResultList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormApprenticeResultList)
{
    ui->setupUi(this);
    setWindowTitle(tr("Ergebnisliste"));

    connect(ui->closeButton, &QPushButton::clicked, this, &FormApprenticeResultList::close);
}

FormApprenticeResultList::~FormApprenticeResultList()
{
    delete ui;
}

QList<ClassLehrling> FormApprenticeResultList::apprenticeList() const
{
    return m_apprenticeList;
}

void FormApprenticeResultList::setapprenticeList(const QList<ClassLehrling> &apprenticeList)
{
    m_apprenticeList = apprenticeList;
    updateTableWidget();
}

void FormApprenticeResultList::updateTableWidget()
{
    if(apprenticeList().isEmpty())
        return;

    QStringList headers;
    headers << "Nr." <<  "Name";
    headers << skillNames(apprenticeList());
    headers << "Ergebnis";

    ui->tableWidget->setColumnCount( headers.size() );
    ui->tableWidget->setHorizontalHeaderLabels( headers );
    ui->tableWidget->setRowCount( apprenticeList().size() );

    QFont f = ui->tableWidget->font();
    f.setBold(true);
    ui->tableWidget->horizontalHeaderItem(headers.size()-1)->setFont(f);

    int row = 0;
    foreach (ClassLehrling appr, apprenticeList())
    {
        QTableWidgetItem *itemNr = new QTableWidgetItem(QString::number( appr.nr(), 10));
        QTableWidgetItem *itemName = new QTableWidgetItem( appr.surname()+","+appr.firstname());

        ui->tableWidget->setItem(row, 0, itemNr);
        ui->tableWidget->setItem(row, 1, itemName);

        itemNr->setFlags(Qt::ItemIsEnabled);
        itemName->setFlags(Qt::ItemIsEnabled);

        double result = 0.0;
        QMap<double, double> resultMap;

        int col = 2;
        foreach (ClassSkills skill, appr.getSkillMap().values())
        {
            //QString skillName = skill.name();
            double percent = skillPercent(skill);
            QTableWidgetItem *itemPercent = new QTableWidgetItem( QString::number(percent, 'g', 3));
            ui->tableWidget->setItem(row, col, itemPercent);
            itemPercent->setFlags(Qt::ItemIsEnabled);
            setupColor(itemPercent, percent);
            resultMap.insert(percent, skill.getWert());

            result += percent * (skill.getWert()/100.0);
            col++;
        }

        QTableWidgetItem *itemResult = new QTableWidgetItem( QString::number(result, 'g', 3));
        ui->tableWidget->setItem(row, col, itemResult);
        itemResult->setFlags(Qt::ItemIsEnabled);
        setupColor(itemResult, result);
        itemResult->setFont(f);
        row++;
    }
    ui->tableWidget->resizeColumnsToContents();
}

void FormApprenticeResultList::setupColor(QTableWidgetItem *item, qreal percent)
{
    QBrush brush = item->foreground();

    if(percent > 49.49)
        brush.setColor(Qt::darkGreen);
    else
        brush.setColor(Qt::red);

    item->setForeground(brush);
}

/// !brief Returns a list of skill names
QStringList FormApprenticeResultList::skillNames(const QList<ClassLehrling> &list)
{
    QStringList nameList;

    foreach (ClassLehrling appr, list)
    {
        foreach (ClassSkills skill, appr.getSkillMap().values())
        {
            if(!nameList.contains( skill.name() ))
                nameList << skill.name();
        }
    }

    return nameList;
}

double FormApprenticeResultList::skillPercent( ClassSkills skill)
{
    double percent = 0.0;

    if(skill.getEvaluationIndex() == 0)
    {
        foreach (ClassProjekt p, skill.getProjektMap().values())
            percent += projectPercent(p) * p.getFactor();

    }

    if(skill.getEvaluationIndex() == 1)
    {
        QStringList list = skill.getIdentifierList();
        for(int i = 0; i < list.size(); i++)
            percent += skill.getIdentPercent(list.at(i)) * skill.getIdentFactor(list.at(i));

        foreach (ClassProjekt pro, skill.getProjektMap().values())
        {
            if(pro.identifierList().isEmpty())
            {
                percent += projectPercent(pro) * pro.getFactor();
            }
        }
    }

    return percent;
}

double FormApprenticeResultList::projectPercent(const ClassProjekt &pro)
{
    double percent = 0.0;
    int points = 0;
    foreach (ClassFrage q, pro.questionMap().values()) {
        points += q.points();
    }

    return percent = points * 100.0 / pro.maxPoints();
}
