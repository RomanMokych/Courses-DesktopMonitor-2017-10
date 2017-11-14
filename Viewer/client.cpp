#include "client.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>
#include <string.h>
#include <qcombobox.h>
VClient::VClient(QObject *parent) :
    QObject(parent)
{
}

void VClient::doConnect()
{
    this->socket_.reset( new QTcpSocket(this));

    connect(this->socket_.data(), SIGNAL(connected()),this, SLOT(connected()));
    connect(this->socket_.data(), SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(this->socket_.data(), SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
    connect(this->socket_.data(), SIGNAL(readyRead()),this, SLOT(readyRead()));

    socket_->connectToHost("127.0.0.1",8000);

    if(!socket_->waitForConnected(5000))
    {
        qDebug() << "Error: " << socket_->errorString();
    }
}

void VClient::connected()
{
    qDebug() << "connected...";

    QJsonObject msg;
    msg.insert("token","JjFaZqOT");
    QJsonDocument doc(msg);
    this->socket_->write(doc.toJson());
}
void VClient::disconnected()
{

}
void VClient::bytesWritten(qint64 bytes)
{

}

void VClient::readyRead()
{
    QByteArray recvd= socket_->readAll();
    qDebug() << recvd;

    if(recvd[0] == char('#'))
    {
        this->ipList = QString(recvd).split('#',QString::SkipEmptyParts);
    }
    if(!this->rFlag_)
    {
        this->rFlag_ = true;
        this->countSendb_ = recvd.toInt();
    }
    else
    {
        QSharedPointer<QGraphicsScene> sc(new QGraphicsScene());
        QPixmap p;
        p.loadFromData(recvd.data(),"PNG");
        sc->addPixmap(p);
        this->scenes_.append(sc);
    }
}

