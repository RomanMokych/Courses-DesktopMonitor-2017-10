#include "mainwindow.h"
#include <QApplication>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.Init();
    w.show();
    return a.exec();
}
