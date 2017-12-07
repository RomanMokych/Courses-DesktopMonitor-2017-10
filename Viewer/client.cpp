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

void VClient::WriteGetFrames(const QString& start_time,
                    const QString& end_time,
                    const QString&ip)
{
    QJsonObject msg;
    msg.insert(qParam::TOKEN, vconfig::TOKEN);
    msg.insert(qParam::FUNC,GET_FRAMESINFO);

    msg.insert(qParam::START_TIME,start_time);
    msg.insert(qParam::END_TIME,end_time);
    msg.insert(qParam::IP,ip);

    QJsonDocument doc(msg);
    this->socket_->write(doc.toJson());
}

void VClient::Connected()
{
    this->SendJSonMessage(GET_IP);

}
void VClient::SendJSonMessage(const viewerCommand f)
{
    QJsonObject msg;
    msg.insert(qParam::FUNC, f);
    msg.insert(qParam::TOKEN, vconfig::TOKEN);
    QJsonDocument doc(msg);
    this->socket_->write(doc.toJson());
}

void VClient::Disconnected()
{
    emit this->ThrowError(guimsg::ERROR_CONNECT);
}

void VClient::Clear()
{
 this->frames.clear();
}

const QString VClient::UnixToLocal(const int& un)
{
    QDateTime timeStamp;
    timeStamp.setTime_t(un);
    QString strTime = timeStamp.toString(vconfig::TIME_FORMAT);

    return strTime;
}
void VClient::ReciveIpResult(const QJsonDocument& msg)
{
    QJsonValue jval = msg.object().value(qParam::LIST_IP);
    QJsonArray iparr = jval.toArray();

    foreach (const QJsonValue &val, iparr) {
        QJsonObject obj = val.toObject();
       this->ip.append(obj.value(qParam::IP).toString());
    }
     emit this->ReadIp();
}

void VClient::ReciveFrameInfo(const QJsonDocument& msg)
{
    QJsonValue Fval = msg.object().value(qParam::SIZE);
    QJsonValue Ftime = msg.object().value(qParam::TIME);
    QJsonValue partCount = msg.object().value(qParam::PART_COUNT);
    this->count_tmp_part_frame_ = partCount.toInt();
    this->count_wait_read_ = Fval.toInt();

    int unixTime = Ftime.toInt();
    QString localtime = this->UnixToLocal(unixTime);
    this->tmp_frame.SetTime(localtime);

    this->SendJSonMessage(READY_FRAME);
    this->frame_flag_ = true;
}

void VClient::RecivePartInfo(const QJsonDocument& msg)
{
    QJsonValue psize = msg.object().value(qParam::SIZE);
    QJsonValue ptime = msg.object().value(qParam::TIME);
    QJsonValue p_lefttop_x = msg.object().value(qParam::LEFTTOP_X);
    QJsonValue p_lefttop_y = msg.object().value(qParam::LEFTTOP_Y);

    this->tmp_frame_x_ = p_lefttop_x.toInt();
    this->tmp_frame_y_ = p_lefttop_y.toInt();
    this->count_wait_read_ = psize.toInt();

    int unixTime = ptime.toInt();
    QString localtime = this->UnixToLocal(unixTime);
    this->tmp_frame.SetTime(localtime);

    this->SendJSonMessage(READY_PART);
    this->part_flag_= true;
}

void VClient::ReciveFramesInfo(const QJsonDocument& msg)
{
    QJsonValue Fval = msg.object().value(qParam::FRAME_COUNT);
    this->count_frame_ = Fval.toInt();
    if(!this->count_frame_)
    {
        emit this->ChangeStatus(guimsg::EMPTY);
        return;
    }

    emit this->Load();

    this->SendJSonMessage(GET_FRAMEINFO);
}

void VClient::RecivePart(const QByteArray& buff)
{
    this->tmp_frame_buff_ +=buff;
    if(this->tmp_frame_buff_.size()< this->count_wait_read_)
    {
        return;
    }

    this->MatchImages();

    this->part_flag_ = false;
    this->count_tmp_part_frame_--;
    if(!this->count_tmp_part_frame_)
    {
        this->tmp_frame_buff_.clear();

        if(this->count_frame_)
        {
            this->SendJSonMessage(GET_FRAMEINFO);
            this->part_flag_ = false;
        }
        else
        {
            emit this->ReadDone();
            return;
        }
    }
    else
    this->SendJSonMessage(GET_PARTINFO);
}
void VClient::MatchImages()
{
    QImage source_image;
    source_image.loadFromData(this->tmp_frame_buff_,vconfig::IMAGEFORMAT);
    QImage result_image = this->frames.last().GetPmap().toImage();

    for(int x = this->tmp_frame_x_; x < this->tmp_frame_x_ + source_image.width(); x++)
        {
            for(int y  = this->tmp_frame_y_; y < this->tmp_frame_y_ + source_image.height(); y++)
            {
                result_image.setPixel(x, y, source_image.pixel(x - this->tmp_frame_x_, y - this->tmp_frame_y_));
            }
        }

    QPixmap tmp_map = QPixmap::fromImage(result_image);

    this->tmp_frame.SetPmap(tmp_map);
    this->tmp_frame_buff_.clear();
    this->frames.append(this->tmp_frame);

}
void VClient::ReciveFrame(const QByteArray& buff)
{
    this->tmp_frame_buff_ += buff;
    if(this->tmp_frame_buff_.size() < this->count_wait_read_)
    {
        return;
    }

    this->tmp_frame.SetPmap(this->tmp_frame_buff_);
    this->tmp_frame_buff_.clear();
    this->frames.append(this->tmp_frame);

    this->count_frame_--;
    this->frame_flag_ = false;
    this->tmp_frame_buff_.clear();
    if(this->count_tmp_part_frame_)
    {
        this->SendJSonMessage(GET_PARTINFO);
    }
    else
    {
        if(!this->count_frame_)
        {

            emit this->ReadDone();
        }
        else
            this->SendJSonMessage(GET_FRAMEINFO);
    }
}
void VClient::ReadyRead()
{
    QByteArray recvd= socket_->readAll();

    if(this->part_flag_)
    {
        this->RecivePart(recvd);
        return;
    }
    else if(this->frame_flag_)
    {
        this->ReciveFrame(recvd);
        return;
    }

    QJsonDocument msg= QJsonDocument::fromJson(recvd);

    QString message = msg.object().value(qParam::FUNC).toString();

    int ms = msg.object().value(qParam::FUNC).toInt();

    switch (ms) {
    case RES_IP:
        this->ReciveIpResult(msg);
        break;
    case RES_FRAME_INFO:
        this->ReciveFrameInfo(msg);
        break;
    case RES_PART_INFO:
         this->RecivePartInfo(msg);
        break;
    case RES_FRAMES_INFO:
         this->ReciveFramesInfo(msg);
        break;
    default:
        break;
    }
}
