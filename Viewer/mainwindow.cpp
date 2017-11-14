#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->client.doConnect();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    this->ui->comboBoxIpList->clear();
    this->ui->comboBoxIpList->insertItems(0,this->client.ipList);
}


void MainWindow::on_pushButton_Show_clicked()
{
   //this->ui->graphicsView_frames->Se
}
