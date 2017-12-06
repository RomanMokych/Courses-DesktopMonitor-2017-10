#include "frame.h"

Frame::Frame(const Frame& f)
{
    this->time_ = f.time_;
    this->pmap_ = f.pmap_;
}
Frame::Frame(QObject *parent) : QObject(parent)
{
    this->setParent(parent);
}
Frame& Frame::operator=(const Frame& f)
{
    Frame res;
    res.pmap_ = f.pmap_;
    res.time_ = f.time_;
}
void Frame::SetTime(const QString time)
{
    this->time_=time;
}
void Frame::SetPmap(const QPixmap& map)
{
    this->pmap_ = map;
}

QString Frame::GetTime()const
{
    return this->time_;
}

void Frame::SetPmap(const QByteArray& buff)
{
    this->pmap_.loadFromData(buff,this->formatName_.toStdString().c_str());
}

QPixmap Frame::GetPmap()const
{
    return this->pmap_;
}
Frame Frame::BuildFrame()
{
    //
}
