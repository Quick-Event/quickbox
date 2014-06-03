
QT += core gui qml widgets
CONFIG += C++11

TEMPLATE = app

TARGET = $$OUT_PWD/../bin/quickevent

LIBS +=      \
	-lqfcore  \
	-lqfqmlwidgets  \

win32: LIBS +=  \
	-L../bin  \

DOLAR=$

unix: LIBS +=  \
	-L../lib  \
	-Wl,-rpath,\'$${DOLAR}$${DOLAR}ORIGIN/../lib\'  \

include($$PWD/src/src.pri)
