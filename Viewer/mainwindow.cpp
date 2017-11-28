#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QObject>              //
#include <QGraphicsPixmapItem>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(&client,SIGNAL(ReadDone()),
                         this,SLOT(Set_scene()));

    QObject::connect(this,SIGNAL(Clear()),
                     &this->client,SLOT(Clear()));

    QObject::connect(&this->client,SIGNAL(ChangeStatus(QString)),
                     this,SLOT(SetStatus(QString)));

    QObject::connect(&this->client,SIGNAL(ThrowError(QString)),
                     this,SLOT(SetError(QString)));

    connect(&client,SIGNAL(ReadIp()),
            this,SLOT(SetIp()));

    connect(&client,SIGNAL(Load()),
            this,SLOT(LoadStart()));



}
void MainWindow::Init()
{
    this->ui->comboBoxIpList->insertItems(0,this->client.ipList);
    this->ui->dateEdit_start->setDate(QDate::currentDate());
    this->ui->dateEdit_end->setDate(QDate::currentDate());
    this->ui->timeEdit_start->setTime(QTime::currentTime());
    this->ui->timeEdit_end->setTime(QTime::currentTime());
    this->client.DoConnect();
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::SetIp()
{
    this->ui->comboBoxIpList->insertItems(0,this->client.ipList);
}
void MainWindow::SetStatus(const QString text)
{
    this->ui->label_status->setText(text);

    QPalette pal = this->ui->label_status->palette();
    pal.setColor(QPalette::WindowText, Qt::black);
    this->ui->label_status->setPalette(pal);
}
void MainWindow::SetError(const QString text)
{
    this->ui->label_status->setText(text);

    QPalette pal = this->ui->label_status->palette();
    pal.setColor(QPalette::WindowText, Qt::red);
    this->ui->label_status->setPalette(pal);

    this->loadScene_.reset();
}
void MainWindow::on_pushButton_clicked()
{
    this->client.DoConnect();
    this->ui->comboBoxIpList->insertItems(0,this->client.ipList);
}

void MainWindow::LoadStart()
{
    this->loadScene_.reset(new QGraphicsScene());
    QGraphicsTextItem *textItem = new QGraphicsTextItem(QString("loading....Plase wait"));
    QFont loadF("Arial",40,10);
    textItem->setFont(loadF);
    textItem->setPos(67, 90);
    this->loadScene_->addItem(textItem);
    this->ui->graphicsView_frames->setScene(this->loadScene_.data());
}

void MainWindow::on_pushButton_Show_clicked()
{
    if(!this->client.Isconnected())
    {
        this->SetError("Error connect");
        this->client.DoConnect();
    }
    emit this->Clear();
    QDateTime start(this->ui->dateEdit_start->date(),this->ui->timeEdit_start->time());
    QDateTime end(this->ui->dateEdit_end->date(),this->ui->timeEdit_end->time());

    QString startT= QString::number(start.toSecsSinceEpoch());
    QString endT =  QString::number(end.toSecsSinceEpoch());
    QString ip = this->ui->comboBoxIpList->currentText();

    this->client.WriteGetFrames(startT,endT,ip);
}

void MainWindow::Set_scene()
{
    this->ui->label_time->setText(this->client.timeList[0]);
    this->ui->horizontalSlider_ch->setEnabled(true);
    this->ui->graphicsView_frames->setScene(this->client.scenes[0].data());
    this->ui->horizontalSlider_ch->setMaximum(this->client.scenes.count()-1);
    this->ui->horizontalSlider_ch->setMinimum(0);

}

void MainWindow::on_horizontalSlider_ch_sliderMoved(int position)
{
    this->ui->graphicsView_frames->setScene(this->client.scenes[position].data());
    this->ui->label_time->setText(this->client.timeList[position]);
}


