#include "webviewcontainer.h"
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QTimer>
#include <QDebug>
#include <QDir>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonValue>
#include <QMessageBox>
#include <QEventLoop>
#include <QApplication>
#include "QCefView/Include/QCefSetting.h"
#include "QCefProtocol.h"
#include "../utils/commonutils.h"

WebViewContainer::WebViewContainer(QWidget *parent)
    : QWidget{parent}
{
    setWindowTitle(" ");
    setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint);
//    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool);
//    SetWindowLong((HWND)winId(), GWL_EXSTYLE, WS_EX_TOOLWINDOW);
    setMinimumSize(806, 456);

    QString customAgent = "UPDF " + QString(APP_VERSION) + " ";
    QCefSetting::setUserAgent(customAgent + QCEF_USER_AGENT);
    QCefSetting::setCachePath(CommonUtils::appWritableDataDir() + QDir::separator() + "CEFCache");

    mCefView = new CefViewWidget(this);
    connect(mCefView, &CefViewWidget::webMessage, this, &WebViewContainer::webMessage);
    connect(mCefView, &CefViewWidget::webClose, this, &WebViewContainer::webClose);
    connect(mCefView, &CefViewWidget::initFail, this, &WebViewContainer::initFail);
    connect(mCefView, &CefViewWidget::getLoginedAccounts, this, &WebViewContainer::getLoginedAccounts);
    connect(mCefView, &CefViewWidget::hideWebView, this, [=] {
        showMinimized();
        hide();
        //处理下次打开保存最后页面的状态
        mCefView->openUrl(mLoginUrl);
    });
//    mCefView->addArchiveResource("", "https://www.xxx.cn/");
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(mCefView);
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);
}

void WebViewContainer::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    mCefView->setFixedSize(this->size().width(), this->size().height());
}

void WebViewContainer::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit closed();
}

void WebViewContainer::openUrl(const QString &url)
{
    qWarning() << "WebViewContainer::openUrl " << url;
    mLoginUrl = url;
    mCefView->openUrl(url);
    activeWebView();
}
void WebViewContainer::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    setFocus();
    QTimer::singleShot(0, this, [=] {
        resize(size().expandedTo(QSize(width()+1,height()+1)));
    });
}

void WebViewContainer::activeWebView()
{
    showMinimized();
    showNormal();
//    if(isMinimized()){
//        if (isMaximized()) {
//            showMaximized();
//        } else {
//            showNormal();
//        }
//    }
//    show();
//    ::SetWindowPos(HWND(winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
//    activateWindow();
//    setFocus();
//    ::SetWindowPos(HWND(winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void WebViewContainer::loginUserInfo(const QString &info)
{
    mCefView->loginUserInfo(info);
}

void WebViewContainer::uninitializeCef()
{
    mCefView->uninitializeCef();
}
