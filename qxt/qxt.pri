SOURCES += \
    qxt/qxtglobal.cpp \
    qxt/qxtglobalshortcut.cpp

HEADERS += \
    qxt/qxtglobal.h \
    qxt/qxtglobalshortcut.h \
    qxt/qxtglobalshortcut_p.h

unix:!macx {
    SOURCES += qxt/x11/qxtglobalshortcut_x11.cpp
    LIBS += -lX11
}
macx {
    SOURCES += qxt/mac/qxtglobalshortcut_mac.cpp
    LIBS += -framework Carbon
}
win32 {
    SOURCES += qxt/win/qxtglobalshortcut_win.cpp
    LIBS += -luser32
}
