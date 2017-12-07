#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QGraphicsScene>
#include <QMainWindow>
#include "frame.h"

enum serverCommand{ RES_IP = 0, RES_FRAME_INFO, RES_PART_INFO, RES_FRAMES_INFO};
enum viewerCommand{ GET_FRAMESINFO =0, GET_FRAMEINFO, GET_PARTINFO, GET_IP, READY_FRAME, READY_PART};
namespace vconfig
{
    //todo: move to config or generate on server
    static const char* IMAGEFORMAT = "PNG";
    static const QString TIME_FORMAT = "ddd MMMM d yy hh:mm:ss";
    static const QString TOKEN= "JjFaZqOT";
    static const QString HOST = "127.0.0.1";
    static const size_t PORT = 8000;
    static const size_t WAIT_TIMEOUT = 5000;
    static const size_t SOCKET_BUFFER_SIZE = 200000;
}

namespace qParam
{
    static const QString TOKEN = "token";
    static const QChar FUNC  = 'F';
    static const QString LIST_IP = "list_ip";
    static const QString IP = "ip";
    static const QString SIZE ="size";
    static const QString TIME = "time";
    static const QString PART_COUNT = "partcount";
    static const QString LEFTTOP_X = "posx";
    static const QString LEFTTOP_Y = "posy";
    static const QString START_TIME = "startT";
    static const QString END_TIME = "endT";
    static const QString FRAME_COUNT = "Fcount";
}

namespace guimsg {
const QString EMPTY ="empty";
const QString ERROR_CONNECT = "Error connection";

}
class VClient:public QObject
{
    Q_OBJECT
public:
    explicit VClient(QMainWindow *parent = 0);
    void DoConnect();
    void WriteGetFrames(const QString& start_time,
               const QString& end_time,
               const QString&ip);

    void SendJSonMessage(const viewerCommand command);

    bool Isconnected()const;
public:
    QStringList ip;
    QList<Frame> frames;
signals:
    void ReadDone();
    void ChangeStatus(const QString text);
    void ThrowError(const QString text);
    void ReadIp();
    void Load();

public slots:
    void Connected();
    void Disconnected();
    void ReadyRead();

private slots:
    void Clear();

private:
    Frame tmp_frame;
    int tmp_frame_x_;
    int tmp_frame_y_;
    int count_wait_read_;
    QByteArray tmp_frame_buff_;
    int count_frame_;
    int count_tmp_part_frame_;
    QSharedPointer<QTcpSocket> socket_;
    bool frame_flag_= false;
    bool part_flag_ = false;

private:
    const QString UnixToLocal(const int& un);
    void ReciveIpResult(const QJsonDocument& msg);
    void ReciveFrameInfo(const QJsonDocument& msg);
    void RecivePartInfo(const QJsonDocument& msg);
    void ReciveFramesInfo(const QJsonDocument& msg);
    void RecivePart(const QByteArray& buff);
    void ReciveFrame(const QByteArray& buff);
    void MatchImages();


};
#endif // CLIENT_H
