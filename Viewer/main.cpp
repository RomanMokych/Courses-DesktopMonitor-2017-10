#include "mainwindow.h"
#include <QApplication>
#include <QObject>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    //QObject::connect(&w.client,SIGNAL(w.client.test1()),
                     //&w,SLOT(w.test2()));

    w.show();
    return a.exec();
}
