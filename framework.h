#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <QObject>
#include <QSharedMemory>
#include <QProcess>
#include <QSocketNotifier>
#include <QFile>

#include "localsocket.h"
#include "localserver.h"
class WebViewContainer;
class Framework : public QObject
{
    Q_OBJECT
public:
    explicit Framework(QObject *parent = nullptr);
    void installLog();
    void initServer();
    void release();
    void createWebView();
private:
    static void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    void onWebClose();
signals:

private slots:
    void onReadData(QByteArray data);
    void onWebMessage(const QString &msg);
    void onEditorQuit();
    void onInitFail();
    void onGetLoginedAccounts();
private:
    static QString mLogPath;
    LocalSocket *mLocalSocket = nullptr;
    LocalServer *mLocalServer = nullptr;

    WebViewContainer *mWebView;
};

#endif // FRAMEWORK_H
