#include "client.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

VClient::VClient(QMainWindow *parent)
{
    this->setParent(parent);
}

void VClient::DoConnect()
{
    this->socket_.reset( new QTcpSocket(this));
    this->socket_->setReadBufferSize(200000);


    connect(this->socket_.data(), SIGNAL(connected()),this, SLOT(Connected()));
    connect(this->socket_.data(), SIGNAL(disconnected()),this, SLOT(Disconnected()));
    //connect(this->socket_.data(), SIGNAL(bytesWritten(qint64)),this, SLOT(BytesWritten(qint64)));
    connect(this->socket_.data(), SIGNAL(readyRead()),this, SLOT(ReadyRead()));

    socket_->connectToHost("127.0.0.1",8000);

    if(!socket_->waitForConnected(5000))
    {
        emit this->ThrowError(socket_->errorString());
        return;
    }
    emit this->ChangeStatus("connect");
}
bool VClient::Isconnected()const
{
    if(this->socket_->isOpen())
    {
        return true;
    }
    else
        return false;
}

void VClient::WriteGetFrames(const QString& startTime,
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

void VClient::WriteGetIp()
{
    QJsonObject msg;
    msg.insert("token",this->token);
    msg.insert("F","getip");

    QJsonDocument doc(msg);
    this->socket_->write(doc.toJson());
}

void VClient::Connected()
{
    qDebug() << "connected...";
    this->WriteGetIp();

}

void VClient::WriteGetTimes()
{
     QJsonObject msg;
     msg.insert("token",this->token);
     msg.insert("F","gettime");
     QJsonDocument doc(msg);
     this->socket_->write(doc.toJson());
}

void VClient::WriteReadyF()
{
     QJsonObject msg;
     msg.insert("token",this->token);
     msg.insert("F","readyF");
     QJsonDocument doc(msg);
     this->socket_->write(doc.toJson());
}

void VClient::Disconnected()
{
    emit this->ThrowError("Error connection");
}

void VClient::Clear()
{
    this->ipList.clear();
    this->timeList.clear();
    this->scenes.clear();
}

const QString VClient::UnixToLocal(const int& un)
{
    QDateTime timeStamp;
    timeStamp.setTime_t(un);
    QString strTime = timeStamp.toString("ddd MMMM d yy hh:mm:ss");

    return strTime;
}

void VClient::ReadyRead()
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
        emit this->ReadIp();
        return;
    }
    else if(msg.object().value("F") == "FRinfo")
    {
        QJsonValue val = msg.object().value("count");
        this->count_frame_ = val.toInt();
        if(!this->count_frame_)
        {
            emit this->ChangeStatus("Empty");
            return;
        }
        this->rFlag_ = true;

        emit this->Load();

        this->WriteGetTimes();
    }
    else if(msg.object().value("F")== "resTime")
    {
        QJsonValue jval = msg.object().value("timeList");
        QJsonArray iparr = jval.toArray();

        foreach (const QJsonValue &val, iparr) {

           int unixTime = val.toString().toInt();
           QString time = this->UnixToLocal(unixTime);
           this->timeList.append(time);
        }

        this->WriteReadyF();
    }
    else if(this->rFlag_)
    {

        this->tmpFrame_+= recvd;
        bool end_file = true;

        for(int i=recvd.size()-5, n=this->endPng_.size()-1;n!=0;--i,--n)
        {
            if(i<5)
            {
                break;
            }
            if((recvd.at(i)) != this->endPng_[n])
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

        p.loadFromData(this->tmpFrame_,"PNG");
        sc->addPixmap(p);

        this->scenes.append(sc);

        qDebug() <<  this->count_frame_;

        this->count_frame_--;
        if(!this->count_frame_)
        {
            this->rFlag_ = false;
            this->tmpFrame_.clear();
            emit this->ReadDone();
        }
        else
        {
            this->tmpFrame_.clear();
            this->WriteReadyF();
        }

    }
}

