#include "framework.h"
#include <iostream>
#include <fstream>
#include <QSettings>
#include <QFile>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <QDateTime>
#include <QStandardPaths>
#include <QApplication>
#include <QMutex>
#include <QMutexLocker>
#include <QTimer>
#include <QDir>
#include "webviewcontainer.h"
#include "../utils/commonutils.h"

Framework::Framework(QObject *parent)
    : QObject{parent}
    , mWebView(nullptr)
{
    installLog();
    qDebug() << "start WebView " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz");
//    initServer(); //  QCefWing server

    mLocalSocket = new LocalSocket(this); // Editor client
    connect(mLocalSocket, &LocalSocket::readBytes, this, &Framework::onReadData);
    connect(mLocalSocket, &LocalSocket::peerDisconnect, this, &Framework::onEditorQuit);

    CommonUtils::killAllProcess("QCefWing.exe");
    createWebView();
}

void Framework::createWebView()
{
    mWebView = new WebViewContainer;
    connect(mWebView, &WebViewContainer::webMessage, this, &Framework::onWebMessage);
    connect(mWebView, &WebViewContainer::webClose, this, &Framework::onWebClose);
    connect(mWebView, &WebViewContainer::initFail, this, &Framework::onInitFail);
    connect(mWebView, &WebViewContainer::getLoginedAccounts, this, &Framework::onGetLoginedAccounts);
}

void Framework::onWebClose()
{
    if (mWebView) {
        delete mWebView;
        mWebView = nullptr;
    }
    createWebView();
}

void Framework::release()
{
    if (mWebView) {
        disconnect(mWebView, &WebViewContainer::webClose, this, &Framework::onWebClose);
        mWebView->uninitializeCef();
        delete mWebView;
        mWebView = nullptr;
    }
}
void Framework::initServer()
{
    mLocalServer = new LocalServer(this);
    connect(mLocalServer,&LocalServer::readBytes,this,&Framework::onReadData);
}

void Framework::onReadData(QByteArray data)
{
    if (!mWebView) {
        mLocalSocket->sendBytes(QString("Rsp:" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz") + " Fail").toLatin1());
        return;
    }
    if (data.startsWith("OpenWeb:")) {
        QString url = data.replace("OpenWeb:", "");
        mWebView->openUrl(url);
    } else if (data.startsWith("LoginedAccounts:")) {
        QString userInfo = data.replace("LoginedAccounts:", "");
        mWebView->loginUserInfo(userInfo);
    } else if (data.startsWith("QuitWebView:")) {
        qDebug() << "~QuitWebView";
        onEditorQuit();
    }

    mLocalSocket->sendBytes(QString("Rsp:" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz")  + " Success").toLatin1());
}

void Framework::onWebMessage(const QString &msg)
{
    mLocalSocket->sendBytes("WebMsg:" + msg.toLatin1());
}

void Framework::onEditorQuit()
{
    qDebug() << "onEditorQuit";
    release();
    qApp->quit();
}

void Framework::onInitFail()
{
    qDebug() << "onInitFail";
    onEditorQuit();
}

void Framework::onGetLoginedAccounts()
{
    mLocalSocket->sendBytes("GetLoginedAccounts");
}

QString Framework::mLogPath = "";
void Framework::installLog()
{
    QString localDataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir dir(localDataPath);
    dir.cdUp();
    QString logDir = dir.path()  + QDir::separator() + APP_NAME;
    if (!QDir(logDir).exists()) {
        QDir().mkpath(logDir);
    }

    mLogPath = logDir + QDir::separator() + "webview-log.txt";
    if (QFileInfo(mLogPath).exists() && QFileInfo(mLogPath).size() / 1024 / 1024 > 10) {
        QFile::remove(mLogPath);
    }
//#ifdef QT_NO_DEBUG
        qInstallMessageHandler(Framework::messageOutput);
//#endif

}

void Framework::messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
    }

    QString message = "";
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    if (context.file != nullptr)
    {
        QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
        message = QString("%1 %2 %3 << %4").arg(text).arg(context_info).arg(current_date_time).arg(msg);
    }
    else
    {
        message = QString("%1 %2 << %3").arg(text).arg(current_date_time).arg(msg);
    }

    QFile file(mLogPath);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();
}
