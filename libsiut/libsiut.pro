#CONFIG += qmake_debug
MY_SUBPROJECT = siut

TEMPLATE = lib
#win32:TARGET = lib$$MY_SUBPROJECT
unix:TARGET = $$OUT_PWD/../lib/$$MY_SUBPROJECT
message(Target: $$TARGET)

QT += sql network serialport

QT	-= gui

CONFIG += qt dll

include(libsiut.pri)



