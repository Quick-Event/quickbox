message("including include/core")

TEMPLATE = lib
TARGET=qfcore
unix:DESTDIR = $$OUT_PWD/../../lib
win32:DESTDIR = $$OUT_PWD/../../bin

QT += xml sql
CONFIG += C++11

DEFINES += QFCORE_BUILD_DLL

include($$PWD/src/src.pri)

include ($$PWD/../../crosscompile-support.pri)

RESOURCES += \
    $${TARGET}.qrc \
