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
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_MainWindow_tabifiedDockWidgetActivated(QDockWidget *dockWidget);

    void on_pushButton_Show_clicked();

private:
    Ui::MainWindow *ui;
    VClient client;
};

#endif // MAINWINDOW_H
