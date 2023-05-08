#CONFIG += qmake_debug

TEMPLATE = lib
TARGET = siut
unix:DESTDIR = $$OUT_PWD/../lib
win32:DESTDIR = $$OUT_PWD/../bin

message(Target: $$TARGET)

QT += sql network serialport
QT	-= gui

CONFIG += qt dll
CONFIG += c++17 hide_symbols
CONFIG += lrelease embed_translations

LIBS += -L$$DESTDIR -lqfcore

include(libsiut.pri)

TRANSLATIONS += \
        libsiut-cs_CZ.ts \
	libsiut-fr_FR.ts \
	libsiut-nb_NO.ts \
	libsiut-nl_BE.ts \
	libsiut-pl_PL.ts \
	libsiut-ru_RU.ts \
	libsiut-uk_UA.ts \
