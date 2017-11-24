#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "client.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    VClient client;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
       void test2();

private slots:

    void on_pushButton_clicked();

    void on_pushButton_Show_clicked();
private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
