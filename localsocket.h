#ifndef LOCALSOCKET_H
#define LOCALSOCKET_H

#include <QObject>
#include <QtNetwork>

class LocalSocket : public QObject
{
    Q_OBJECT
public:
    explicit LocalSocket(QObject *parent = nullptr);
    void sendBytes(const QByteArray &data);
signals:
    void readBytes(const QByteArray &data);
    void peerDisconnect();
private slots:
    void onReadReady();
    void onSocketError(QAbstractSocket::SocketError error);
    void onPeerDisconnect();
private:
    QTcpSocket *mSocket;
};

#endif // LOCALSOCKET_H
