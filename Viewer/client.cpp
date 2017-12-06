#include "client.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QPainter>
#include <QImage>


VClient::VClient(QMainWindow *parent)
{
    this->setParent(parent);
}

void VClient::DoConnect()
{
    this->socket_.reset( new QTcpSocket(this));
    this->socket_->setReadBufferSize(vconfig::SOCKET_BUFFER_SIZE);

    connect(this->socket_.data(), SIGNAL(connected()),this, SLOT(Connected()));
    connect(this->socket_.data(), SIGNAL(disconnected()),this, SLOT(Disconnected()));
    connect(this->socket_.data(), SIGNAL(readyRead()),this, SLOT(ReadyRead()));

    socket_->connectToHost(vconfig::HOST, vconfig::PORT);

    if(!socket_->waitForConnected(vconfig::WAIT_TIMEOUT))
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
    msg.insert(qParam::TOKEN, vconfig::TOKEN);
    msg.insert(qParam::FUNC,GETFRAMESINFO);

    msg.insert(qParam::STARTT,startTime);
    msg.insert(qParam::ENDT,endTime);
    msg.insert(qParam::IP,ip);

    QJsonDocument doc(msg);
    this->socket_->write(doc.toJson());
}

void VClient::Connected()
{
    this->SendJSonMessage(GETIP);

}
void VClient::SendJSonMessage(const viewerCommand f)
{
    QJsonObject msg;
    msg.insert(qParam::FUNC, vconfig::TOKEN);
    msg.insert(qParam::FUNC, f);
    QJsonDocument doc(msg);
    this->socket_->write(doc.toJson());
}

void VClient::Disconnected()
{
    emit this->ThrowError(guimsg::ERCONN);
}

void VClient::Clear()
{
 this->frames.clear();
}

const QString VClient::UnixToLocal(const int& un)
{
    QDateTime timeStamp;
    timeStamp.setTime_t(un);
    QString strTime = timeStamp.toString(vconfig::TIMEFORMAT);

    return strTime;
}
void VClient::ReciveIpResult(const QJsonDocument& msg)
{
    QJsonValue jval = msg.object().value(qParam::LISTIP);
    QJsonArray iparr = jval.toArray();

    foreach (const QJsonValue &val, iparr) {
        QJsonObject obj = val.toObject();
       this->ipList.append(obj.value(qParam::IP).toString());
    }
     emit this->ReadIp();
}

void VClient::ReciveFrameInfo(const QJsonDocument& msg)
{
    QJsonValue Fval = msg.object().value(qParam::SIZE);
    QJsonValue Ftime = msg.object().value(qParam::TIME);
    QJsonValue partCount = msg.object().value(qParam::PARTCOUNT);
    this->count_tmpPartFrame_ = partCount.toInt();
    this->countWaitRead_ = Fval.toInt();

    int unixTime = Ftime.toInt();
    QString localtime = this->UnixToLocal(unixTime);
    this->tmpFrame.SetTime(localtime);

    this->SendJSonMessage(READYF);
    this->rFFlag_ = true;
}

void VClient::RecivePartInfo(const QJsonDocument& msg)
{
    QJsonValue Psize = msg.object().value(qParam::SIZE);
    QJsonValue Ptime = msg.object().value(qParam::TIME);
    QJsonValue Pposx = msg.object().value(qParam::POSX);
    QJsonValue Pposy = msg.object().value(qParam::POSY);

    this->tmpFrameX_ = Pposx.toInt();
    this->tmpFrameY_ = Pposy.toInt();
    this->countWaitRead_ = Psize.toInt();

    int unixTime = Ptime.toInt();
    QString localtime = this->UnixToLocal(unixTime);
    this->tmpFrame.SetTime(localtime);

    this->SendJSonMessage(READYP);
    this->rPflag_= true;
}

void VClient::ReciveFramesInfo(const QJsonDocument& msg)
{
    QJsonValue Fval = msg.object().value(qParam::FCOUNT);
    this->count_frame_ = Fval.toInt();
    if(!this->count_frame_)
    {
        emit this->ChangeStatus(guimsg::EMPTY);
        return;
    }

    emit this->Load();

    this->SendJSonMessage(GETFINFO);
}

void VClient::RecivePart(const QByteArray& buff)
{
    this->tmpFrameBuff_ +=buff;
    if(this->tmpFrameBuff_.size()< this->countWaitRead_)
    {
        return;
    }

    this->MatchImages();

    this->rPflag_ = false;
    this->count_tmpPartFrame_--;
    if(!this->count_tmpPartFrame_)
    {
        this->tmpFrameBuff_.clear();

        if(this->count_frame_)
        {
            this->SendJSonMessage(GETFINFO);
            this->rPflag_ = false;
        }
        else
        {
            emit this->ReadDone();
            return;
        }
    }
    else
    this->SendJSonMessage(GETPINFO);
}
void VClient::MatchImages()
{
    QImage sourceImage;
    sourceImage.loadFromData(this->tmpFrameBuff_,vconfig::IMFORMAT);
    QImage resultImage = this->frames.last().GetPmap().toImage();

    for(int x = this->tmpFrameX_; x < this->tmpFrameX_ + sourceImage.width(); x++)
        {
            for(int y  = this->tmpFrameY_; y < this->tmpFrameY_ + sourceImage.height(); y++)
            {
                resultImage.setPixel(x, y, sourceImage.pixel(x - this->tmpFrameX_, y - this->tmpFrameY_));
            }
        }

    QPixmap p = QPixmap::fromImage(resultImage);

    this->tmpFrame.SetPmap(p);
    this->tmpFrameBuff_.clear();
    this->frames.append(this->tmpFrame);

}
void VClient::ReciveFrame(const QByteArray& buff)
{
    this->tmpFrameBuff_+= buff;
    if(this->tmpFrameBuff_.size() < this->countWaitRead_)
    {
        return;
    }

    this->tmpFrame.SetPmap(this->tmpFrameBuff_);
    this->tmpFrameBuff_.clear();
    this->frames.append(this->tmpFrame);
    this->lastFIndex = this->frames.count()-1;

    this->count_frame_--;
    this->rFFlag_ = false;
    this->tmpFrameBuff_.clear();
    if(this->count_tmpPartFrame_)
    {
        this->SendJSonMessage(GETPINFO);
    }
    else
    {
        if(!this->count_frame_)
        {

            emit this->ReadDone();
        }
        else
            this->SendJSonMessage(GETFINFO);
    }
}
void VClient::ReadyRead()
{
    QByteArray recvd= socket_->readAll();

    if(this->rPflag_)
    {
        this->RecivePart(recvd);
        return;
    }
    else if(this->rFFlag_)
    {
        this->ReciveFrame(recvd);
        return;
    }

    QJsonDocument msg= QJsonDocument::fromJson(recvd);

    QString message = msg.object().value(qParam::FUNC).toString();

    int ms = msg.object().value(qParam::FUNC).toInt();

    switch (ms) {
    case RESIP:
        this->ReciveIpResult(msg);
        break;
    case RESFINFO:
        this->ReciveFrameInfo(msg);
        break;
    case RESPINFO:
         this->RecivePartInfo(msg);
        break;
    case RESFRAMESINFO:
         this->ReciveFramesInfo(msg);
        break;
    default:
        break;
    }
}
