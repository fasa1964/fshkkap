#include "fshkwindow.h"
#include <QApplication>
#include <QGuiApplication>
#include <QSplashScreen>
#include <QLocale>
#include <QTimer>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QGuiApplication::setApplicationName("FSHK-AP");
    QGuiApplication::setApplicationVersion("1.0");


    // testing
//    QSettings settings("FASA-Soft", "FSHK-AP");
//    if(settings.value("checkupdate").toBool())
//    {
//        QPixmap pix = QPixmap(":/images/SplashPixmap.svg");
//        QSplashScreen splash(pix, Qt::WindowStaysOnTopHint);
//        QFont font = splash.font();
//        font.setPointSize(10);
//        splash.setFont(font);
//        splash.setMinimumSize(500,500);
//        splash.show();

//        splash.showMessage("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
//                           "FSHK-AP suche nach udates..", Qt::AlignCenter);
//        app.processEvents();

//        QTimer::singleShot(8000, &splash, &QWidget::close);
//    }

    FSHKWindow window;
    window.show();


    //splash->finish(&window);

    return app.exec();
}
