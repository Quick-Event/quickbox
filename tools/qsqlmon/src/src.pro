message(including $$PWD)
QT += core gui qml widgets sql xml

CONFIG += C++11

TEMPLATE = app

DESTDIR = $$OUT_PWD/../../../bin
TARGET = qsqlmon

LIBS +=      \
	-lqfcore  \
	-lqfqmlwidgets  \

win32: LIBS +=  \
	-L../../../bin  \

include ($$PWD/../../../crosscompile-support.pri)

DOLAR=$

unix: LIBS +=  \
	-L../../../lib  \
	-Wl,-rpath,\'$${DOLAR}$${DOLAR}ORIGIN/../lib\'  \

INCLUDEPATH += $$PWD/../../../libqf/libqfcore/include
INCLUDEPATH += $$PWD/../../../libqf/libqfqmlwidgets/include

include($$PWD/src.pri)

