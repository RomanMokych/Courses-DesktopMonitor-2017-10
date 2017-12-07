#include "frame.h"

Frame::Frame(const Frame& f)
{
    this->time_ = f.time_;
    this->map_ = f.map_;
}
Frame::Frame(QObject *parent) : QObject(parent)
{
    this->setParent(parent);
}
Frame& Frame::operator=(const Frame& frame)
{
    Frame res;
    res.map_ = frame.map_;
    res.time_ = frame.time_;
}
void Frame::SetTime(const QString time)
{
    this->time_=time;
}
void Frame::SetPmap(const QPixmap& map)
{
    this->map_ = map;
}

QString Frame::GetTime()const
{
    return this->time_;
}

void Frame::SetPmap(const QByteArray& buff)
{
    this->map_.loadFromData(buff,this->format_name_.toStdString().c_str());
}

QPixmap Frame::GetPmap()const
{
    return this->map_;
}
