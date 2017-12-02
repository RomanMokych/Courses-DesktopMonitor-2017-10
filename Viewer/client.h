#ifndef CLIENT_H
#define CLIENT_H


#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>           // //////////////////////////////////////
#include <QGraphicsScene>
#include <QMainWindow>

class VClient:public QObject
{
    Q_OBJECT
public:
    explicit VClient(QMainWindow *parent = 0);
    void DoConnect();
    void WriteGetFrames(const QString& startTime,
               const QString& endTime,
               const QString&ip);

    void WriteGetIp();
    void WriteReadyF();
    void WriteGetTimes();

    bool Isconnected()const;
public:
    QStringList ipList;
    QStringList timeList;
    QList<QSharedPointer<QGraphicsScene>> scenes;

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
    QByteArray tmpFrame_;
    int count_frame_;
    QSharedPointer<QTcpSocket> socket_;
    bool rFlag_= false;


private:
    const QString UnixToLocal(const int& un);
};
#endif // CLIENT_H
