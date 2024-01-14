QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = WebView
TEMPLATE = app

DEFINES += APP_NAME=\\\"$$TARGET\\\"
VERSION = 1.0.3.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/../
INCLUDEPATH += $$PWD/../utils/
INCLUDEPATH += $$PWD/../cef/include/
INCLUDEPATH += $$PWD/../QCefProto/
INCLUDEPATH += $$PWD/../QCefView/

CONFIG(release, debug|release) {
    LIBS += -L$$PWD/../build/release/ -lQCefView
}
CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../build/debug/ -lQCefView
}
LIBS += -luser32

SOURCES += \
    cefviewwidget.cpp \
    webviewcontainer.cpp \
    framework.cpp \
    localserver.cpp \
    localsocket.cpp \
    main.cpp \


HEADERS += \
    cefviewwidget.h \
    webviewcontainer.h \
    framework.h \
    localserver.h \
    localsocket.h

include($$PWD/../build.pri)
include($$PWD/../utils/utils_h.pri)


CONFIG(debug, debug|release){
    LIBS += -L$$PWD/../build/debug -lutilsd
} else {
    LIBS += -L$$PWD/../build/release -lutils
}

RC_ICONS=$$PWD/../editor/updf.ico
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
