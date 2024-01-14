#ifndef WEBVIEWCONTAINER_H
#define WEBVIEWCONTAINER_H

#include <QWidget>
#include "cefviewwidget.h"

class WebViewContainer : public QWidget
{
    Q_OBJECT
public:
    explicit WebViewContainer(QWidget *parent = nullptr);
    void openUrl(const QString& url);
    void uninitializeCef();
    void activeWebView();
    void loginUserInfo(const QString &info);
protected:
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);
signals:
    void webMessage(const QString &msg);
    void webClose();
    void initFail();

    void closed();
    void getLoginedAccounts();
protected slots:
private:
    CefViewWidget *mCefView;
    QRegion draggableRegion_;
    QRegion nonDraggableRegion_;
    QString mLoginUrl;
};

#endif // WEBVIEWCONTAINER_H
