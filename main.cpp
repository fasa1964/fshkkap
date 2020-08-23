#include "fshkwindow.h"
#include <QApplication>
#include <QGuiApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QGuiApplication::setApplicationName("FSHK-AP");
    QGuiApplication::setApplicationVersion("1.0");


    FSHKWindow window;
    window.show();

    return app.exec();
}
