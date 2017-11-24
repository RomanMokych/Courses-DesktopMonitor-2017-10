#include "client.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>
#include <string.h>
#include <qcombobox.h>
#include <QJsonObject>
#include <QJsonArray>
VClient::VClient(QMainWindow *parent)
{
    this->setParent(parent);
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

    this->endPng = QByteArray("IEND");
}

void VClient::writeGetFrames(const QString& startTime,
                    const QString& endTime,
                    const QString&ip)
{
    QJsonObject msg;
    msg.insert("token",this->token);
    msg.insert("F","getframes");

    msg.insert("startT",startTime);
    msg.insert("endT",endTime);
    msg.insert("ip",ip);

    QJsonDocument doc(msg);
    this->socket_->write(doc.toJson());
}

void VClient::writeGetIp()
{
    QJsonObject msg;
    msg.insert("token",this->token);
    msg.insert("F","getip");

    QJsonDocument doc(msg);
    this->socket_->write(doc.toJson());
}

void VClient::connected()
{
    qDebug() << "connected...";
    this->writeGetIp();

}

void VClient::writeReadyF()
{
     QJsonObject msg;
     msg.insert("token",this->token);
     msg.insert("F","readyF");
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

    QJsonDocument msg= QJsonDocument::fromJson(recvd);

    if(msg.object().value("F") == "IpResult")
    {
       QJsonValue jval = msg.object().value("list_ip");
       QJsonArray iparr = jval.toArray();

       foreach (const QJsonValue &val, iparr) {
           QJsonObject obj = val.toObject();
          this->ipList.append(obj.value("ip").toString());
       }

        return;
    }
    else if(msg.object().value("F") == "resFCount")
    {
        QJsonValue val = msg.object().value("count");
        this->count_frame = val.toInt();
        qDebug() <<this->count_frame;
        this->rFlag_ = true;
        this->writeReadyF();
    }
    else if(this->rFlag_)
    {

        this->tmpFrame+= recvd;
        bool end_file = true;

        this->countstep++;// ///////////////////
        for(int i=recvd.size()-5, n=this->endPng.size()-1;n!=0;--i,--n)
        {
            if((recvd.at(i)) != this->endPng[n])
            {
                end_file= false;
                break;
            }
        }

        if(end_file==false)
        {
            return;
        }

        QSharedPointer<QGraphicsScene> sc(new QGraphicsScene());
        QPixmap p;
        p.loadFromData(this->tmpFrame.data(),"PNG");
        sc->addPixmap(p);
        this->scenes_.append(sc);

        this->count_frame--;
        if(!this->count_frame)
        {
            this->rFlag_ = false;
            this->tmpFrame.clear();
        }
        else
            this->tmpFrame.clear();
            this->writeReadyF();

    }
}

