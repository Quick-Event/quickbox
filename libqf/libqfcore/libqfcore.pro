message("including include/core")

TEMPLATE = lib
unix:TARGET = $$OUT_PWD/../../lib/qfcore

QT +=
CONFIG += C++11

DEFINES += QFCORE_BUILD_DLL

include($$PWD/src/src.pri)

include ($$PWD/../../crosscompile-support.pri)
