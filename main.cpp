#include "mainwindow.h"
#include <QApplication>
#include "canlib_class.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    Canlib *lib = new Canlib();
    canStatus stat;

    lib->canInitializeLibrary();
    stat = lib->canOpenChannel(0, 0);

    w.show();

    return a.exec();
}
