#include "dwclient.h"

DWClient::DWClient(MainWindow obj,const QString &conStr):conStr(connstr)
{
    this->socket= new QTcpSocket(this);
    this->connectToHost(1,2);
}

bool DWClient::Write(const QString& message)
{
   int res =this->socket.sendTextMessage(message);

}

