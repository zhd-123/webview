#include "localsocket.h"
#include <QDebug>
#include <QApplication>

LocalSocket::LocalSocket(QObject *parent)
    : QObject{parent}
{
    mSocket = new QTcpSocket(this);
    connect(mSocket, &QTcpSocket::readyRead, this, &LocalSocket::onReadReady);
    connect(mSocket, &QTcpSocket::errorOccurred, this, &LocalSocket::onSocketError);
    connect(mSocket, &QTcpSocket::disconnected, this, &LocalSocket::onPeerDisconnect);
    mSocket->connectToHost("127.0.0.1", 22681);
}

void LocalSocket::sendBytes(const QByteArray &data)
{
    if (!mSocket) {
        return;
    }
    mSocket->write(data);
}

void LocalSocket::onReadReady()
{
    emit readBytes(mSocket->readAll());
}

void LocalSocket::onSocketError(QAbstractSocket::SocketError error)
{
    qWarning() << "LocalSocket Error:" << error;
}

void LocalSocket::onPeerDisconnect()
{
    qWarning() << "editor disconnect";
    if (mSocket) {
        delete mSocket;
        mSocket = nullptr;
    }
    // 关闭工作内容，进行退出
    emit peerDisconnect();
}
