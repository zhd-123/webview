#include "localserver.h"

LocalServer::LocalServer(QObject *parent)
    : QObject{parent}
    , mConnectSocket(nullptr)
{
    mServer = new QTcpServer(this);
    connect(mServer, &QTcpServer::newConnection, this, &LocalServer::onNewConnection);
    mServer->listen(QHostAddress::Any, 12681);
}

LocalServer::~LocalServer()
{
    if (mServer) {
        mServer->close();
        mServer = nullptr;
    }
}

void LocalServer::sendBytes(const QByteArray &data)
{
    if (!mConnectSocket) {
        return;
    }
    mConnectSocket->write(data);
}

void LocalServer::onNewConnection()
{
    mConnectSocket = mServer->nextPendingConnection();
    connect(mConnectSocket, &QTcpSocket::readyRead, this, &LocalServer::onReadReady);
    connect(mConnectSocket, &QTcpSocket::disconnected, this, &LocalServer::onPeerDisconnect);
}

void LocalServer::onReadReady()
{
    emit readBytes(mConnectSocket->readAll());
}

void LocalServer::onPeerDisconnect()
{

}
