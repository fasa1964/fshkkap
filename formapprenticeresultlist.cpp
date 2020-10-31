#include "formapprenticeresultlist.h"
#include "ui_formapprenticeresultlist.h"

FormApprenticeResultList::FormApprenticeResultList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormApprenticeResultList)
{
    ui->setupUi(this);
    connect(ui->closeButton, &QPushButton::clicked, this, &FormApprenticeResultList::close);
}

FormApprenticeResultList::~FormApprenticeResultList()
{
    delete ui;
}

QList<ClassLehrling> FormApprenticeResultList::apprtenticeList() const
{
    return m_apprtenticeList;
}

void FormApprenticeResultList::setApprtenticeList(const QList<ClassLehrling> &apprtenticeList)
{
    m_apprtenticeList = apprtenticeList;
    updateTableWidget();
}

void FormApprenticeResultList::updateTableWidget()
{
    if(apprtenticeList().isEmpty())
        return;

    QStringList headers;
    headers << "Nr." <<  "Name";
    headers << skillNames(apprtenticeList());

    ui->tableWidget->setColumnCount( headers.size() );
    ui->tableWidget->setHorizontalHeaderLabels( headers );
    ui->tableWidget->setRowCount( apprtenticeList().size() );

    int row = 0;
    foreach (ClassLehrling appr, apprtenticeList())
    {
        QTableWidgetItem *itemNr = new QTableWidgetItem(QString::number( appr.nr(), 10));
        QTableWidgetItem *itemName = new QTableWidgetItem( appr.surname()+","+appr.firstname());

        ui->tableWidget->setItem(row, 0, itemNr);
        ui->tableWidget->setItem(row, 1, itemName);

        itemNr->setFlags(Qt::ItemIsEnabled);
        itemName->setFlags(Qt::ItemIsEnabled);

        int col = 2;
        foreach (ClassSkills skill, appr.getSkillMap().values())
        {
            if(skill.getEvaluationIndex() == 0)
            {
                double percent = skillPercent(skill);
                QTableWidgetItem *itemPercent = new QTableWidgetItem( QString::number(percent, 'g', 3));
                ui->tableWidget->setItem(row, col, itemPercent);
                itemPercent->setFlags(Qt::ItemIsEnabled);
            }

            if(skill.getEvaluationIndex() == 1 && !skill.getIdentifierList().isEmpty()  )
            {
                QStringList iList = skill.getIdentifierList();
                for(int i = 0; i < iList.size() ; i++)
                {
                    double percent = skill.getIdentPercent(iList.at(i));
                    QTableWidgetItem *itemPercent = new QTableWidgetItem( QString::number(percent, 'g', 3) );
                    ui->tableWidget->setItem(row, col, itemPercent);
                    itemPercent->setFlags(Qt::ItemIsEnabled);
                }


            }
            col++;
        }



        row++;
    }


    ui->tableWidget->resizeColumnsToContents();

}

/// !brief Returns a list of skill names
QStringList FormApprenticeResultList::skillNames(const QList<ClassLehrling> &list)
{
    QStringList nameList;

    foreach (ClassLehrling appr, list)
    {
        foreach (ClassSkills skill, appr.getSkillMap().values())
        {
            if(skill.getEvaluationIndex() != 1)
            {
                if(!nameList.contains( skill.name()))
                    nameList << skill.name();
            }
            if(skill.getEvaluationIndex() == 1 && !skill.getIdentifierList().isEmpty() )
            {
                QStringList iList = skill.getIdentifierList();
                foreach (QString s, iList)
                {
                    if(!nameList.contains(s))
                        nameList << s;
                }

                foreach (ClassProjekt pro, skill.getProjektMap().values())
                {
                    if(pro.identifierList().isEmpty())
                    {
                        if( !nameList.contains( pro.name() ))
                            nameList << pro.name();
                    }
                }
            }
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
