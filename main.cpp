#include "fshkwindow.h"
#include <QApplication>
#include <QGuiApplication>
#include <QSplashScreen>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QGuiApplication::setApplicationName("FSHK-AP");
    QGuiApplication::setApplicationVersion("1.0");

    QPixmap pix = QPixmap(":/images/SplashPixmap.svg");

    // testing
//    QSplashScreen *splash = new QSplashScreen (pix);
//    splash->setMinimumSize(500,500);
//    splash->show();

//    splash->showMessage("FSHK-AP startet...lade Module");
//    app.processEvents();

//    splash->showMessage("FSHK-AP Module sind geladen!");
//    app.processEvents();

    FSHKWindow window;
    window.show();


    //splash->finish(&window);

    return app.exec();
}
