#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QObject>              //
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->client.doConnect();

    QObject::connect(&client,SIGNAL(test1()),
                         this,SLOT(test2()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    this->ui->comboBoxIpList->insertItems(0,this->client.ipList);
}

void MainWindow::on_pushButton_Show_clicked()
{
    QString startT= QString::number(this->ui->dateTimeEdit_start->dateTime().toSecsSinceEpoch());
    QString endT =  QString::number(this->ui->dateTimeEdit_end->dateTime().toSecsSinceEpoch());
    QString ip = this->ui->comboBoxIpList->currentText();

    this->client.writeGetFrames(startT,endT,ip);
}

void MainWindow::test2()
{
    qDebug() <<"eee booy";

}
