#ifndef CUSTOMCEFVIEW_H
#define CUSTOMCEFVIEW_H

#include <QScreen>
#include <QTimer>
#include "include/QCefView.h"

/// <summary>
/// Represents the customized QCefView
/// </summary>
class CefViewWidget : public QCefView
{
  Q_OBJECT

public:
  CefViewWidget(QWidget* parent = 0);

  ~CefViewWidget();
  void openUrl(const QString& url);
  void changeColor();
  void loginUserInfo(const QString &info);
protected:
  void resizeEvent(QResizeEvent* event);

signals:
    void webMessage(const QString &msg);
    void hideWebView();
    void webClose();
    void initFail();

    void tryStopTimer();
    void getLoginedAccounts();
private slots:
    void onCheckInitTimeout();
    void onTryStopTimer();
protected:
  virtual void onAfterCreated();
  virtual void onBeforeClose();
  // false 取消请求 headMap入参
  virtual bool onBeforeResourceLoad(const QString& url, const QString& method, QMap<QString, QString> *headMap);

  virtual void onLoadStart() override;
  virtual void onLoadEnd(int httpStatusCode) override;
  virtual void onLoadError(int errorCode, const QString& errorMsg, const QString& failedUrl, bool& handled) override;
  virtual void onConsoleMessage(const QString& message, int level) override;

  virtual void onDraggableRegionChanged(const QRegion& region) override;
  virtual void onQCefUrlRequest(const QString& url) override;
  virtual void onQCefQueryRequest(const QCefQuery& query) override;
  virtual void onInvokeMethodNotify(int browserId, int frameId, const QString& method, const QVariantList& arguments) override;

private:
  bool mCreated; // check browser is ready
  QList<QString> mUrlsBfCreate;
  QTimer mCheckInitTimer;
  QCefQuery mCacheQuery;
};

#endif // CUSTOMCEFVIEW_H
