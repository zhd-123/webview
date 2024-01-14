QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
DEFINES += QCEFVIEW_LIB
TARGET = QCefView

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
include($$PWD/../build.pri)
include($$PWD/../cef/cef.pri)

INCLUDEPATH += $$PWD/../QCefProto
INCLUDEPATH += $$PWD/../QCefWing

#CONFIG(release, debug|release) {
#    LIBS += $$PWD/../build/Release/QCefProto.lib
#}
#CONFIG(debug, debug|release) {
#    LIBS += $$PWD/../build/Debug/QCefProto.lib
#}

QMAKE_CXXFLAGS_RELEASE += /Zi
QMAKE_CXXFLAGS_RELEASE += /Od
QMAKE_LFLAGS_RELEASE += /DEBUG

LIBS += -luser32 -lgdi32

SOURCES += \
    CCefManager.cpp \
    CCefSetting.cpp \
    CCefWindow.cpp \
    CefViewBrowserApp/BrowserDelegates/QCefViewDefaultBrowserDelegate.cpp \
    CefViewBrowserApp/QCefQueryHandler.cpp \
    CefViewBrowserApp/QCefViewBrowserApp.cpp \
    CefViewBrowserApp/QCefViewBrowserAppHelper.cpp \
    CefViewBrowserApp/QCefViewBrowserHandler.cpp \
    CefViewBrowserApp/SchemeHandlers/QCefViewDefaultSchemeHandler.cpp \
    QCefEvent.cpp \
    QCefQuery.cpp \
    QCefSetting.cpp \
    QCefView.cpp

HEADERS += \
    CCefManager.h \
    CCefSetting.h \
    CCefWindow.h \
    CefViewBrowserApp/BrowserDelegates/QCefViewDefaultBrowserDelegate.h \
    CefViewBrowserApp/QCefQueryHandler.h \
    CefViewBrowserApp/QCefViewBrowserApp.h \
    CefViewBrowserApp/QCefViewBrowserHandler.h \
    CefViewBrowserApp/QCefViewDelegate.h \
    CefViewBrowserApp/SchemeHandlers/QCefViewDefaultSchemeHandler.h \
    Include/QCefEvent.h \
    Include/QCefQuery.h \
    Include/QCefSetting.h \
    Include/QCefView.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    CMakeLists.txt \
    QCefView.rc
