#include "mySocket.h"

mySocket::mySocket(QObject *parent)
    : QObject(parent)
    , m_Socket(new QTcpSocket)
{
    initalSocket();
}

mySocket::~mySocket()
{
    delete m_Socket;
}

void mySocket::handleInfo(const QString& account,const QString& passwd)
{
    m_Socket->connectToHost("192.168.167.42",801);
}

void mySocket::initalSocket()
{
    ;
}