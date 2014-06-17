message("including $$PWD")

TEMPLATE = lib
unix:TARGET = $$OUT_PWD/../../lib/qfqmlwidgets

QT += widgets qml

CONFIG += C++11

DEFINES += QFQMLWIDGETS_BUILD_DLL

LIBS +=      \
	-lqfcore  \

win32: LIBS +=  \
	-L../../bin  \

unix: LIBS +=  \
	-L../../lib  \

include($$PWD/src/src.pri)
