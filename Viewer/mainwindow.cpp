#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QObject>
#include <QGraphicsPixmapItem>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&client,SIGNAL(ReadDone()),
                         this,SLOT(Set_scene()));

    connect(this,SIGNAL(Clear()),
                     &this->client,SLOT(Clear()));

    connect(&this->client,SIGNAL(ChangeStatus(QString)),
                     this,SLOT(SetStatus(QString)));

    connect(&this->client,SIGNAL(ThrowError(QString)),
                     this,SLOT(SetError(QString)));

    connect(&client,SIGNAL(ReadIp()),
            this,SLOT(SetIp()));

    connect(&client,SIGNAL(Load()),
            this,SLOT(LoadStart()));
}
void MainWindow::Init()
{
    this->ui->comboBoxIpList->insertItems(0,this->client.ip);
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
    this->ui->comboBoxIpList->insertItems(0,this->client.ip);
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

    this->load_scene_.reset();
}
void MainWindow::on_pushButton_clicked()
{
    this->client.DoConnect();
    this->ui->comboBoxIpList->insertItems(0,this->client.ip);
}

void MainWindow::LoadStart()
{
    this->load_scene_.reset(new QGraphicsScene());
    QGraphicsTextItem *textItem = new QGraphicsTextItem(QString("loading....Plase wait"));
    QFont loadF("Arial",40,10);
    textItem->setFont(loadF);
    textItem->setPos(67, 90);
    this->load_scene_->addItem(textItem);
    this->ui->graphicsView_frames->setScene(this->load_scene_.data());
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
    this->current_scene_.reset(new QGraphicsScene());
    this->current_scene_->addPixmap(this->client.frames.first().GetPmap());

    this->ui->label_time->setText(this->client.frames.first().GetTime());
    this->ui->horizontalSlider_ch->setEnabled(true);
    this->ui->graphicsView_frames->setScene(this->current_scene_.data());
    this->ui->horizontalSlider_ch->setMaximum(this->client.frames.count()-1);
    this->ui->horizontalSlider_ch->setMinimum(0);

}

void MainWindow::on_horizontalSlider_ch_sliderMoved(int position)
{
    this->current_scene_.reset(new QGraphicsScene());
    this->current_scene_->addPixmap(this->client.frames[position].GetPmap());

    this->ui->graphicsView_frames->setScene(this->current_scene_.data());
    this->ui->label_time->setText(this->client.frames[position].GetTime());
}


