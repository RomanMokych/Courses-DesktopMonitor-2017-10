#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QGraphicsScene>
#include <QMainWindow>
#include "frame.h"

enum serverCommand{ RESIP = 0, RESFINFO, RESPINFO, RESFRAMESINFO};
enum viewerCommand{ GETFRAMESINFO =0, GETFINFO, GETPINFO, GETIP, READYF, READYP};
namespace vconfig
{
    //todo: move to config or generate on server
    static const char* IMFORMAT = "PNG";
    static const QString TIMEFORMAT = "ddd MMMM d yy hh:mm:ss";
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
    static const QString LISTIP = "list_ip";
    static const QString IP = "ip";
    static const QString SIZE ="size";
    static const QString TIME = "time";
    static const QString PARTCOUNT = "partcount";
    static const QString POSX = "posx";
    static const QString POSY = "posy";
    static const QString STARTT = "startT";
    static const QString ENDT = "endT";
    static const QString FCOUNT = "Fcount";
}

namespace guimsg {
const QString EMPTY ="empty";
const QString ERCONN = "Error connection";

}
class VClient:public QObject
{
    Q_OBJECT
public:
    explicit VClient(QMainWindow *parent = 0);
    void DoConnect();
    void WriteGetFrames(const QString& startTime,
               const QString& endTime,
               const QString&ip);

    void SendJSonMessage(const viewerCommand com);

    bool Isconnected()const;
public:
    QStringList ipList;
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
    Frame tmpFrame;
    int tmpFrameX_;
    int tmpFrameY_;
    int countWaitRead_;
    QByteArray tmpFrameBuff_;
    int count_frame_;
    int count_tmpPartFrame_;
    QSharedPointer<QTcpSocket> socket_;
    bool rFFlag_= false;
    bool rPflag_ = false;
    int lastFIndex= 0;

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
