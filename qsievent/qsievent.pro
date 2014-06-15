message(including $$PWD)
QT += core gui qml widgets
CONFIG += C++11

TEMPLATE = app

TARGET = $$OUT_PWD/../bin/qsievent

LIBS +=      \
	-lqfcore  \

win32: LIBS +=  \
	-L../bin  \

DOLAR=$

unix: LIBS +=  \
	-L../lib  \
	-Wl,-rpath,\'$${DOLAR}$${DOLAR}ORIGIN/../lib\'  \

include($$PWD/src/src.pri)
