message(including $$PWD)
QT += core gui qml widgets

CONFIG += C++11

TEMPLATE = app

TARGET = $$OUT_PWD/../bin/qsievent

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

OTHER_FILES += \
    divers/qsievent/plugins/Core/*.qml \
    divers/qsievent/plugins/SqlDb/*.qml \
    divers/qsievent/plugins/Event/*.qml \
    divers/qsievent/plugins/Help/*.qml \

QML_IMPORT_PATH += \
    $$OUT_PWD/../lib/qml \
    $$PWD/divers/qsievent/plugins \
