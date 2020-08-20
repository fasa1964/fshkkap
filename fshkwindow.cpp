#include "fshkwindow.h"
#include "ui_fshkwindow.h"

#include <QSettings>
#include <QMessageBox>



FSHKWindow::FSHKWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FSHKWindow)
{
    ui->setupUi(this);
    readSettings();
    appwidget = new AppWidget(qApp->applicationName(), qApp->applicationVersion(), this);
    appwidget->hide();

    setApplicationLabel();


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

}

void FSHKWindow::actionApprenticeClicked()
{

}

void FSHKWindow::actionProjectClicked()
{

}

void FSHKWindow::setApplicationLabel()
{
    this->takeCentralWidget();
    appwidget->show();
    setCentralWidget(appwidget);
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
