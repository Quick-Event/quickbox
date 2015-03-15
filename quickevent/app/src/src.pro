message(including $$PWD)

QT += core gui qml widgets

CONFIG += C++11

TEMPLATE = app

QF_PROJECT_TOP_SRCDIR = $$PWD/../../..
QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/../../..

DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/bin
TARGET = quickevent

win32: LIB_DIR_NAME = bin
else:  LIB_DIR_NAME = lib

INCLUDEPATH += \
$$QF_PROJECT_TOP_SRCDIR/libqf/libqfcore/include \
$$QF_PROJECT_TOP_SRCDIR/libqf/libqfqmlwidgets/include \

message(INCLUDEPATH: $$INCLUDEPATH)

LIBS += \
	-L$$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME \

LIBS +=      \
	-lqfcore  \
	-lqfqmlwidgets  \

unix: LIBS +=  \
	-L../../../lib  \
	-Wl,-rpath,\'\$\$ORIGIN/../lib:\$\$ORIGIN/../lib/qml/quickevent\'  \

include ($$QF_PROJECT_TOP_SRCDIR/crosscompile-support.pri)

include($$PWD/src.pri)

OTHER_FILES += \
    ../plugins/qml/Core/*.qml \
    ../plugins/qml/SqlDb/*.qml \
    ../plugins/qml/Event/*.qml \
    ../plugins/qml/Help/*.qml \

QML_IMPORT_PATH += \
    $$QF_PROJECT_TOP_BUILDDIR/lib/qml \
	$$QF_PROJECT_TOP_BUILDDIR/lib/qml/quickevent \
