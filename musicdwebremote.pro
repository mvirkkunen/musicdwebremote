TARGET = musicdwebremote
TEMPLATE = app

QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    main.cpp \
    eventhttpserver.cpp \
    popupwindow.cpp \
    trackinfo.cpp

HEADERS += \
    eventhttpserver.h \
    popupwindow.h \
    trackinfo.h

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    README.md \
    COPYING \
    popupwindow.qss

include(qxt/qxt.pri)
