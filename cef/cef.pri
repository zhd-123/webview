INCLUDEPATH += $$PWD


CONFIG(release, debug|release) {
    LIBS += $$PWD/lib/Release/libcef_dll_wrapper.lib
    LIBS += $$PWD/lib/Release/libcef.lib
    LIBS += $$PWD/lib/Release/QCefProto.lib
}
CONFIG(debug, debug|release) {
    LIBS += $$PWD/lib/Debug/libcef_dll_wrapper.lib
    LIBS += $$PWD/lib/Debug/libcef.lib
    LIBS += $$PWD/lib/Debug/QCefProto.lib
}
