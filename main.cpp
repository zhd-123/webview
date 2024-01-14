#include "framework.h"
#include <QApplication>

int main(int argc, char *argv[])
{
#if(QT_VERSION >= QT_VERSION_CHECK(5,6,0))
   QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
   QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#if(QT_VERSION >= QT_VERSION_CHECK(5,15,2))
   QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
#endif

    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    Framework w;
    return a.exec();
}
