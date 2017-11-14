#ifndef CLIENT_H
#define CLIENT_H


#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>
#include <QGraphicsScene>
class VClient:public QObject
{
    Q_OBJECT
public:
    explicit VClient(QObject *parent = 0);
    void doConnect();
public:
    QStringList ipList;

public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();

private:
    QSharedPointer<QTcpSocket> socket_;
    bool rFlag_= false;
    int countSendb_ = 0;
    QVector<QSharedPointer<QGraphicsScene>> scenes_;

};
#endif // CLIENT_H
