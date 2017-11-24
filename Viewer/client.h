#ifndef CLIENT_H
#define CLIENT_H


#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>
#include <QGraphicsScene>
#include <QMainWindow>
class VClient:public QObject
{
    Q_OBJECT
public:
    explicit VClient(QMainWindow *parent = 0);
    void doConnect();
    void writeGetFrames(const QString& startTime,
               const QString& endTime,
               const QString&ip);
    void writeGetIp();
    void writeReadyF();

public:
    QStringList ipList;
    QList<QSharedPointer<QGraphicsScene>> scenes_;
    QGraphicsView * view;


signals:
    void test1();

public slots:
    void connected();
    void disconnected();

    void readyRead();
    void bytesWritten(qint64 bytes);
private:
    QByteArray endPng;
    QByteArray tmpFrame;
    int count_frame;
    const QString token= "JjFaZqOT";
    QSharedPointer<QTcpSocket> socket_;
    bool rFlag_= false;
    bool readyToreadB= false;
    int countSendb_ = 0;

    int countstep= 0;//del
};
#endif // CLIENT_H
