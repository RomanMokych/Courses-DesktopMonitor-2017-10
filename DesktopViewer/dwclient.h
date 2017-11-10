#ifndef DWCLIENT_H
#define DWCLIENT_H

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>

class DWClient:public QObject
{
    Q_OBJECT
public:
    DWClient(MainWindow obj,const QString &conStr);
    bool Write();

private:
    QString conStr;
    QTcpSocket socket;
};

#endif // DWCLIENT_H
