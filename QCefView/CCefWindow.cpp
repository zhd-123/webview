#pragma region qt_headers
#include <QCoreApplication>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QPaintDevice>
#include <QPainter>
#include <QDebug>
#pragma endregion qt_headers

#include "CCefWindow.h"
#include "CCefManager.h"

CCefWindow::CCefWindow(QCefView* view /*= 0*/)
  : QWindow(view->windowHandle())
  , view_(view)
  , hwndCefBrowser_(nullptr)
{   
  setFlags(Qt::FramelessWindowHint);
  CCefManager::getInstance().initializeCef();
}

CCefWindow::~CCefWindow()
{
  if (hwndCefBrowser_)
    hwndCefBrowser_ = nullptr;

//  CCefManager::getInstance().uninitializeCef();
}

void
CCefWindow::setCefBrowserWindow(CefWindowHandle hwnd)
{
  hwndCefBrowser_ = hwnd;
  syncCefBrowserWindow();
}

void
CCefWindow::onLoadingStateChanged(bool isLoading, bool canGoBack, bool canGoForward)
{
  if (view_)
      view_->onLoadingStateChanged(isLoading, canGoBack, canGoForward);
}

void CCefWindow::onAfterCreated()
{
    if (view_)
        view_->onAfterCreated();
}

void CCefWindow::onBeforeClose()
{
    if (view_)
        view_->onBeforeClose();
}

bool CCefWindow::onBeforeResourceLoad(const CefString &url, const CefString &method, CefRequest::HeaderMap *headMap)
{
    int ret = true;
    if (view_) {
        QMap<QString, QString> map;
        for (auto it = headMap->begin(); it != headMap->end(); ++it) {
            CefString cefKey = (*it).first;
            CefString cefValue = (*it).second;
            map.insert(QString::fromStdString(cefKey.ToString()), QString::fromStdString(cefValue.ToString()));
        }
        (*headMap).clear();
        ret = view_->onBeforeResourceLoad(QString::fromStdString(url.ToString()), QString::fromStdString(method.ToString()), &map);
        for (QString key : map.keys()) {
            CefString cefKey,cefValue;
            cefKey.FromString(key.toStdString());
            cefValue.FromString(map.value(key).toStdString());
            (*headMap).insert(std::pair<CefString, CefString>(cefKey, cefValue));
        }
    }
    return ret;
}

void
CCefWindow::onLoadStart()
{
  if (view_)
    view_->onLoadStart();
}

void
CCefWindow::onLoadEnd(int httpStatusCode)
{
  if (view_)
    view_->onLoadEnd(httpStatusCode);
}

void
CCefWindow::onLoadError(int errorCode, const CefString& errorMsg, const CefString& failedUrl, bool& handled)
{
  if (view_) {
    auto msg = QString::fromStdString(errorMsg.ToString());
    auto url = QString::fromStdString(failedUrl.ToString());
    view_->onLoadError(errorCode, msg, url, handled);
  }
}

void
CCefWindow::onDraggableRegionChanged(const std::vector<CefDraggableRegion> regions)
{
  if (view_) {
    // Determine new draggable region.
    QRegion region;
    std::vector<CefDraggableRegion>::const_iterator it = regions.begin();
    for (; it != regions.end(); ++it) {
      region += QRegion(it->bounds.x, it->bounds.y, it->bounds.width, it->bounds.height);
    }
    view_->onDraggableRegionChanged(region);
  }
}

void
CCefWindow::onConsoleMessage(const CefString& message, int level)
{
  if (view_) {
    auto msg = QString::fromStdString(message.ToString());
    view_->onConsoleMessage(msg, level);
  }
}

void
CCefWindow::onTakeFocus(bool next)
{
  if (view_)
    view_->onTakeFocus(next);
}

void
CCefWindow::onQCefUrlRequest(const CefString& url)
{
  if (view_) {
    auto u = QString::fromStdString(url.ToString());
    view_->onQCefUrlRequest(u);
  }
}

void
CCefWindow::onQCefQueryRequest(const CefString& request, int64 query_id)
{
  if (view_) {
    auto req = QString::fromStdString(request.ToString());
    view_->onQCefQueryRequest(QCefQuery(req, query_id));
  }
}

void
CCefWindow::onInvokeMethodNotify(int browserId, const CefRefPtr<CefListValue>& arguments)
{
  if (!view_)
    return;

  if (arguments && (arguments->GetSize() >= 2)) {
    int idx = 0;
    if (CefValueType::VTYPE_INT == arguments->GetType(idx)) {
      int frameId = arguments->GetInt(idx++);

      if (CefValueType::VTYPE_STRING == arguments->GetType(idx)) {
        CefString functionName = arguments->GetString(idx++);
        if (functionName == QCEF_INVOKEMETHOD) {
          QString method;
          if (CefValueType::VTYPE_STRING == arguments->GetType(idx)) {
#if defined(CEF_STRING_TYPE_UTF16)
            method = QString::fromWCharArray(arguments->GetString(idx++).c_str());
#elif defined(CEF_STRING_TYPE_UTF8)
            method = QString::fromUtf8(arguments->GetString(idx++).c_str());
#elif defined(CEF_STRING_TYPE_WIDE)
            method = QString::fromWCharArray(arguments->GetString(idx++).c_str());
#endif
          }

          QVariantList argumentList;
          QString qStr;
          for (idx; idx < arguments->GetSize(); idx++) {
            if (CefValueType::VTYPE_NULL == arguments->GetType(idx))
              argumentList.push_back(0);
            else if (CefValueType::VTYPE_BOOL == arguments->GetType(idx))
              argumentList.push_back(QVariant::fromValue(arguments->GetBool(idx)));
            else if (CefValueType::VTYPE_INT == arguments->GetType(idx))
              argumentList.push_back(QVariant::fromValue(arguments->GetInt(idx)));
            else if (CefValueType::VTYPE_DOUBLE == arguments->GetType(idx))
              argumentList.push_back(QVariant::fromValue(arguments->GetDouble(idx)));
            else if (CefValueType::VTYPE_STRING == arguments->GetType(idx)) {
#if defined(CEF_STRING_TYPE_UTF16)
              qStr = QString::fromWCharArray(arguments->GetString(idx).c_str());
#elif defined(CEF_STRING_TYPE_UTF8)
              qStr = QString::fromUtf8(arguments->GetString(idx).c_str());
#elif defined(CEF_STRING_TYPE_WIDE)
              qStr = QString::fromWCharArray(arguments->GetString(idx).c_str());
#endif
              argumentList.push_back(qStr);
            } else {
            }
          }

          view_->onInvokeMethodNotify(browserId, frameId, method, argumentList);
        }
      }
    }
  }
}

void
CCefWindow::syncCefBrowserWindow()
{
  double w = width() * devicePixelRatio();
  double h = height() * devicePixelRatio();
  if (hwndCefBrowser_)
    ::SetWindowPos(hwndCefBrowser_, NULL, 0, 0, w, h, SWP_NOZORDER | SWP_NOSENDCHANGING | SWP_DEFERERASE);
}

void
CCefWindow::exposeEvent(QExposeEvent* e)
{
  syncCefBrowserWindow();
  return __super::exposeEvent(e);
}

void
CCefWindow::resizeEvent(QResizeEvent* e)
{
  syncCefBrowserWindow();
  __super::resizeEvent(e);

  return;
}

bool CCefWindow::event(QEvent *e)
{
    return QWindow::event(e);
}

bool CCefWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    if (eventType == "windows_generic_MSG")
    {
        MSG* winMsg = static_cast<MSG *>(message);
        switch (winMsg->message) {
        case WM_SYSCOMMAND: {
            if (SC_CLOSE == winMsg->wParam)//禁止任务栏"关闭窗口"
            {
                return true;
            } else if (SC_MINIMIZE == winMsg->wParam) //窗口最小化
            {
                int aa = 0;
            } else if (SC_MAXIMIZE == winMsg->wParam) //窗口最大化
            {
                int aa = 0;
            }
        }
            break;
        }
    }
    return false;
}
