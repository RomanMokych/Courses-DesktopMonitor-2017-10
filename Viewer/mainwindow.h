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
    void Init();

signals:
    void Clear();

public slots:
       void Set_scene();
       void SetStatus(const QString text);
       void SetError(const QString text);
       void SetIp();
       void LoadStart();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_Show_clicked();
    void on_horizontalSlider_ch_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
    QSharedPointer<QGraphicsScene> loadScene_;
    QSharedPointer<QGraphicsScene> currentScene_;
};

#endif // MAINWINDOW_H
