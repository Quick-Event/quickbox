#CONFIG += qmake_debug

TEMPLATE = lib
TARGET = siut
unix:DESTDIR = $$OUT_PWD/../lib
win32:DESTDIR = $$OUT_PWD/../bin

message(Target: $$TARGET)

QT += sql network serialport
QT	-= gui

CONFIG += qt dll
CONFIG += c++11 hide_symbols

LIBS += -L$$DESTDIR -lqfcore

include(libsiut.pri)

TRANSLATIONS += \
    libsiut.cs_CZ.ts \
    libsiut.nb_NO.ts \
    libsiut.ru_RU.ts \


