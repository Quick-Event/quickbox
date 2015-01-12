message(including $$PWD)

QT += core gui qml widgets

CONFIG += C++11

TEMPLATE = app

DESTDIR = $$OUT_PWD/../../bin/
TARGET = quickevent

INCLUDEPATH += \
$$PWD/../../libqf/libqfcore/include \
$$PWD/../../libqf/libqfqmlwidgets/include \

message(INCLUDEPATH: $$INCLUDEPATH)

LIBS +=      \
	-lqfcore  \
	-lqfqmlwidgets  \

win32: LIBS += \
	-L../../bin \

DOLAR=$

unix: LIBS +=  \
	-L../../lib  \
	-Wl,-rpath,\'\$\$ORIGIN/../lib\'  \

include ($$PWD/../../crosscompile-support.pri)

include($$PWD/src.pri)

OTHER_FILES += \
    ../plugins/qml/Core/*.qml \
    ../plugins/qml/SqlDb/*.qml \
    ../plugins/qml/Event/*.qml \
    ../plugins/qml/Help/*.qml \

QML_IMPORT_PATH += \
    $$OUT_PWD/../lib/qml \
	$$OUT_PWD/../lib/qml/quickevent \
