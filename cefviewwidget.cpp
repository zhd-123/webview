#include "CefViewWidget.h"
#include <QDebug>
#include <QColor>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QResizeEvent>
#include <windows.h>

CefViewWidget::CefViewWidget(QWidget* parent /* = 0*/)
    : QCefView(parent)
    , mCreated(false)
{
    connect(this, &CefViewWidget::tryStopTimer, this, &CefViewWidget::onTryStopTimer);

    mCheckInitTimer.setInterval(2000);
    mCheckInitTimer.setSingleShot(true);
    connect(&mCheckInitTimer, &QTimer::timeout, this, &CefViewWidget::onCheckInitTimeout);
    mCheckInitTimer.start();
    setContextMenuPolicy(Qt::CustomContextMenu);
}

CefViewWidget::~CefViewWidget()
{

}

void CefViewWidget::openUrl(const QString &url)
{
    if (!mCreated) {
        mUrlsBfCreate.append(url);
        qWarning() << "CefViewWidget::openUrl unCreated" << url;
        return;
    }
    navigateToUrl(url);
}

void CefViewWidget::resizeEvent(QResizeEvent* event)
{
    QCefView::resizeEvent(event);
}

void CefViewWidget::onCheckInitTimeout()
{
    // 白屏有onAfterCreated()，但是没有进入onLoadStart()
    qDebug() << "onCheckInitTimeout";
    mCheckInitTimer.stop();
    emit initFail();
}

void
CefViewWidget::changeColor()
{
  QColor color(QRandomGenerator::global()->generate());

  QCefEvent event("colorChange");
  event.setStringProperty("color", color.name());
  broadcastEvent(event);
}

void CefViewWidget::onAfterCreated()
{
    qWarning() << "onAfterCreated()";
    mCreated = true;
    if (!mUrlsBfCreate.isEmpty()) {
        for (QString url : mUrlsBfCreate) {
            openUrl(url);
        }
        mUrlsBfCreate.clear();
    }
}

void CefViewWidget::onBeforeClose()
{
    emit webClose();
    qWarning() << "onBeforeClose()";
}

bool CefViewWidget::onBeforeResourceLoad(const QString &url, const QString &method, QMap<QString, QString> *headMap)
{
//    qDebug() << "url:" << url << " method:" << method;
    if (url.contains("winupdf://")) {
        if (url.contains("winupdf://closeWindow")) {
            emit hideWebView();
        } else {
            emit webMessage(url);
        }
        return false;
    }
    for (QString key : headMap->keys()) {
        //qDebug() << "headMap key:" << key << " value:" << headMap->value(key);
    }
//    QString customAgent = "UPDF " + QString(APP_VERSION()) + " ";
//    if (headMap->contains("User-Agent")) {
//        customAgent += headMap->value("User-Agent");
//    }
//    *headMap->insert("User-Agent", customAgent);
    return true;
}

void CefViewWidget::onLoadStart()
{
    emit tryStopTimer();
    qDebug() << "onLoadStart()";
}
void CefViewWidget::onTryStopTimer()
{
    if (mCheckInitTimer.isActive()) {
        mCheckInitTimer.stop();
    }
}

void CefViewWidget::onLoadEnd(int httpStatusCode)
{
    qDebug() << "onLoadEnd() code:" << httpStatusCode;
}

void CefViewWidget::onLoadError(int errorCode, const QString &errorMsg, const QString &failedUrl, bool &handled)
{
    qWarning() << "onLoadError() errorCode:" << errorCode << " errorMsg:" << errorMsg << " failedUrl:" << failedUrl;
}

void CefViewWidget::onConsoleMessage(const QString &message, int level)
{
    qWarning() << "onConsoleMessage() message:" << message;
}

void
CefViewWidget::onDraggableRegionChanged(const QRegion& region)
{}

void
CefViewWidget::onQCefUrlRequest(const QString& url)
{
  QString title("QCef Url Request");
  QString text = QString("Current Thread: QT_UI\r\n"
                         "Url: %1")
                   .arg(url);

//  QMetaObject::invokeMethod(this, [=]() {
//      QMessageBox::information(this->window(), title, text);
//  }, Qt::QueuedConnection);
}

void
CefViewWidget::onQCefQueryRequest(const QCefQuery& query)
{
//  QString title("QCef Query Request");
//  QString text = QString("Current Thread: QT_UI\r\n"
//                         "Query: %1")
//                   .arg(query.reqeust());

//  QMetaObject::invokeMethod(this, [=]() {
//    QMessageBox::information(this->window(), title, text);
//  }, Qt::QueuedConnection);
  if ("getLoginedAccounts" == query.reqeust()) {
      emit getLoginedAccounts();
      mCacheQuery = query;
  }
}

void CefViewWidget::loginUserInfo(const QString &info)
{
    mCacheQuery.setResponseResult(true, info);
    responseQCefQuery(mCacheQuery);
//    QString title("QCef Query Response");
//    QMetaObject::invokeMethod(this, [=]() {
//        QMessageBox::information(this->window(), title, info);
//    }, Qt::QueuedConnection);
}

void
CefViewWidget::onInvokeMethodNotify(int browserId, int frameId, const QString& method, const QVariantList& arguments)
{
  if (0 == method.compare("onDragAreaMouseDown")) {
    HWND hWnd = ::GetAncestor((HWND)getCefWinId(), GA_ROOT);

    // get current mouse cursor position
    POINT pt;
    ::GetCursorPos(&pt);

    // in case the mouse is being captured, try to release it
    ::ReleaseCapture();

    // simulate that the mouse left button is down on the title area
    ::SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, POINTTOPOINTS(pt));
    return;
  }

  QString title("QCef InvokeMethod Notify");
  QString text = QString("Current Thread: QT_UI\r\n"
                         "Method: %1\r\n"
                         "Arguments: ...")
                   .arg(method);

//  QMetaObject::invokeMethod(this, [=]() {
//    QMessageBox::information(this->window(), title, text);
//  }, Qt::QueuedConnection);
}

