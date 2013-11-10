TARGET = musicdwebremote
TEMPLATE = app

QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    main.cpp \
    eventhttpserver.cpp

HEADERS += \
    eventhttpserver.h

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    README.md \
    COPYING

include(qxt/qxt.pri)
