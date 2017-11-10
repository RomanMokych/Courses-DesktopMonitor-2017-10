#ifndef SVCLIENT_H
#define SVCLIENT_H
#include <QString>
#include <QTcpSocket>


class SVClient
{
    Q_OBJECT
public:
    SVClient();
    SVClient(QObject* obj,const QString&host,const QString&port);
    void write(const QString& message);
private:

};

#endif // SVCLIENT_H
