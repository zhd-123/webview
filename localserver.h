#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <QObject>
#include <QtNetwork>

class LocalServer : public QObject
{
    Q_OBJECT
public:
    explicit LocalServer(QObject *parent = nullptr);
    ~LocalServer();
    void sendBytes(const QByteArray &data);
signals:
    void readBytes(const QByteArray &data);
private slots:
    void onNewConnection();
    void onReadReady();
    void onPeerDisconnect();
private:
    QTcpServer *mServer;
    QTcpSocket *mConnectSocket;
};

#endif // LOCALSERVER_H
