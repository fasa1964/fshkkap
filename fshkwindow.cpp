#include "fshkwindow.h"
#include "ui_fshkwindow.h"

#include <QSettings>
#include <QMessageBox>
#include <QFileInfo>
#include <QFile>
#include <QFileDevice>
#include <QDataStream>


FSHKWindow::FSHKWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FSHKWindow)
{
    ui->setupUi(this);
    readSettings();

    readDatas("Betriebe.dat");
    readDatas("Lehrlinge.dat");

    appwidget = new AppWidget(qApp->applicationName(), qApp->applicationVersion(), this);
    appwidget->hide();

    setApplicationLabel();

    formCompany = new FormBetrieb(this);
    formCompany->hide();
    connect(formCompany, &FormBetrieb::companyFormClosed, this, &FSHKWindow::formHasClosed);
    connect(formCompany, &FormBetrieb::saveCompanyMap, this, &FSHKWindow::saveCompanyMap);

    formApprentice = new FormLehrling(this);
    formApprentice->hide();
    connect(formApprentice, &FormLehrling::apprenticeFormClosed, this, &FSHKWindow::formHasClosed);
    connect(formApprentice, &FormLehrling::saveApprenticeMap, this, &FSHKWindow::saveApprenticeMap);
    connect(formApprentice, &FormLehrling::apprenticeWithoutCompany, this, &FSHKWindow::apprenticeWithoutCompany);
    connect(formApprentice, &FormLehrling::apprenticeHasCompany , this, &FSHKWindow::apprenticeAssociatedCompany );

    connect(ui->actionBeenden, &QAction::triggered, this, &FSHKWindow::actionCloseClicked);
    connect(ui->actionInfo, &QAction::triggered, this, &FSHKWindow::actionInfoClicked);
    connect(ui->actionCompany, &QAction::triggered, this, &FSHKWindow::actionCompanyClicked);
    connect(ui->actionApprentice, &QAction::triggered, this, &FSHKWindow::actionApprenticeClicked);
    connect(ui->actionProjects, &QAction::triggered, this, &FSHKWindow::actionProjectClicked);
}

FSHKWindow::~FSHKWindow()
{
    delete ui;
}

void FSHKWindow::actionCloseClicked()
{
    writeSettings();
    close();
}

void FSHKWindow::actionInfoClicked()
{
    QMessageBox::information(this, tr("Info"), tr("Not available yet!"));
}

void FSHKWindow::actionCompanyClicked()
{
    this->takeCentralWidget();
    formCompany->setCompanyMap(companyMap);
    formCompany->setLastModified( lastFileModified("Betriebe.dat"));
    formCompany->show();
    setCentralWidget(formCompany);
}

void FSHKWindow::actionApprenticeClicked()
{
    this->takeCentralWidget();
    formApprentice->setApprenticeMap(apprenticeMap);
    formApprentice->setCompanyMap(companyMap);
    formApprentice->setLastModified( lastFileModified("Lehrlinge.dat"));
    formApprentice->show();
    setCentralWidget(formApprentice);
}

void FSHKWindow::actionProjectClicked()
{

}

void FSHKWindow::formHasClosed()
{
    setApplicationLabel();
}

/// Signals from FormsCompany
void FSHKWindow::saveCompanyMap(const QMap<int, ClassBetrieb> &cMap)
{
    companyMap = cMap;
    saveDatas("Betriebe.dat");
}

/// Signals from FormsApprentice
void FSHKWindow::saveApprenticeMap(const QMap<QString, ClassLehrling> &aMap)
{
    apprenticeMap = aMap;
    saveDatas("Lehrlinge.dat");
}

/// !brief Signal emitter when editing an apprentice
/// and the company edit field is empty or the apprentice was deleting
void FSHKWindow::apprenticeWithoutCompany(const ClassLehrling &appr)
{
    bool dataChanged = false;
    QMapIterator<int, ClassBetrieb> it(companyMap);
    while (it.hasNext()) {
        it.next();
        ClassBetrieb comp = it.value();
        QMap<QString, ClassLehrling> aMap = comp.azubiMap();
        if(aMap.keys().contains(appr.getKey())){
            aMap.remove(appr.getKey());
            comp.setAzubiMap(aMap);
            companyMap.insert(comp.nr(), comp);
            dataChanged = true;
        }
    }

    if(dataChanged)
        saveDatas("Betriebe.dat");
}

/// !brief Signal emitter when editing an apprentice
/// and the company edit field was changed
void FSHKWindow::apprenticeAssociatedCompany(const QString &company, const QString &apprenticeKey)
{
    // Insert the apprentice into company
    ClassBetrieb comp = getCompany(company);
    ClassLehrling appr = apprenticeMap.value(apprenticeKey);
    QMap<QString, ClassLehrling> aMap = comp.azubiMap();
    aMap.insert(appr.getKey(), appr);
    comp.setAzubiMap(aMap);
    companyMap.insert(comp.nr(), comp);

    // Check if apprentice still in other company
    // if yes delete the apprentice from all other company
    QMapIterator<int, ClassBetrieb> it(companyMap);
    while (it.hasNext()) {
        it.next();
        if(it.value().azubiMap().keys().contains(apprenticeKey)){
            if(it.value().nr() != comp.nr()){
                ClassBetrieb co = it.value();
                QMap<QString, ClassLehrling> aMap = co.azubiMap();
                aMap.remove(apprenticeKey);
                co.setAzubiMap(aMap);
                companyMap.insert(co.nr(), co);
            }
        }

    }

    saveDatas("Betriebe.dat");
}


void FSHKWindow::setApplicationLabel()
{
    this->takeCentralWidget();
    appwidget->show();
    setCentralWidget(appwidget);
}

/// !brief Return ClassCompany seaching by name
ClassBetrieb FSHKWindow::getCompany(const QString &name)
{
    ClassBetrieb comp;
    QMapIterator<int, ClassBetrieb> it(companyMap);
    while (it.hasNext()) {
        it.next();
        if(it.value().name() == name){
            comp = it.value();
            break;
        }
    }

    return comp;
}

void FSHKWindow::readDatas(const QString &filename)
{
    QFile file(filename);
    if(!file.exists())
        return;

    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,  tr("Fehler Laden"),
                     tr("Die Datei: ")+ filename + tr("lässt sich nicht zum Lesen öffnen.\n") +
                      "Fehler: " + file.errorString());
        return;
    }

    QDataStream in(&file);
    quint32 magic;
    quint32 version;
    in >> magic;
    if(magic != 13101964){
        QMessageBox::warning(this, tr("Dateifehler"), tr("Die Datei hat nicht das richtige Format!"));
        return;
    }

    in >> version;
    if(version != QDataStream::Qt_5_9){
        QMessageBox::warning(this, tr("Dateifehler"), tr("Die Datei hat nicht die richtige Version!"));
        return;
    }

    while (!in.atEnd())
    {
        if(filename == "Betriebe.dat")
        {
            ClassBetrieb company;
            in >> company;
            companyMap.insert(company.nr(), company);
        }

        if(filename == "Lehrlinge.dat")
        {
            ClassLehrling appr;
            in >> appr;
            apprenticeMap.insert(appr.getKey(), appr);
        }
    }

    file.close();
}

bool FSHKWindow::saveDatas(const QString &filename)
{
    QFile file(filename);

    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this,  tr("Fehler Laden"),
                     tr("Die Datei: ")+ filename + tr("lässt sich nicht zum Schreiben öffnen.\n") +
                      "Fehler: " + file.errorString());
        return false;
    }

    QDataStream out(&file);
    quint32 magic = 13101964;
    quint32 version = QDataStream::Qt_5_9;
    out << magic;
    out << version;

    if(filename == "Betriebe.dat")
    {
        QMapIterator<int, ClassBetrieb> it(companyMap);
        while (it.hasNext()) {
            it.next();
            out << it.value();
        }
    }
    if(filename == "Lehrlinge.dat")
    {
        QMapIterator<QString, ClassLehrling> it(apprenticeMap);
        while (it.hasNext()) {
            it.next();
            out << it.value();
        }
    }

    file.close();
    return true;
}

QDateTime FSHKWindow::lastFileModified(const QString &filename)
{
    QDateTime dt;
    QFileInfo file(filename);
    dt = file.lastModified();
    return dt;
}

/// !brief Read the settings
/// from last season
void FSHKWindow::readSettings()
{
    QSettings settings ("FASA-Soft", "FSHK-AP");
    QRect wRect = settings.value("geometrie", QRect(0,0,930,640)).toRect();
    this->setGeometry(wRect);
}

/// !brief Write the settings
/// from this season
void FSHKWindow::writeSettings()
{
    QSettings settings ("FASA-Soft", "FSHK-AP");
    settings.setValue("geometrie", this->geometry());

}
