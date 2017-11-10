#include "svclient.h"

SVClient::SVClient(QObject* obj,const QString&host,const QString&port)
{
    this->sock = new QTcpSocket(this);
    this->sock->connectToHost(host,port);
}

void SVClient::write(const QString& message)
{
    this->sock->write("hello");
}
SVClient::SVClient(){}
