message("including $$PWD")

TEMPLATE = lib
TARGET=qfqmlwidgets

QF_PROJECT_BUILD_ROOT = $(QF_PROJECT_BUILD_ROOT)
isEmpty(QF_PROJECT_BUILD_ROOT) {
	QF_PROJECT_BUILD_ROOT = $$OUT_PWD/../..
}

unix:DESTDIR = $$QF_PROJECT_BUILD_ROOT/lib
win32:DESTDIR = $$QF_PROJECT_BUILD_ROOT/bin

message ( DESTDIR: $$DESTDIR )

QT += widgets qml sql
QT += xml printsupport svg # needed by reports

CONFIG += c++11
CONFIG += hide_symbols

#QMAKE_CXXFLAGS += -std=c++1y

DEFINES += QFQMLWIDGETS_BUILD_DLL

LIBS +=      \
	-lqfcore  \

win32: LIBS +=  \
	-L../../bin  \

unix: LIBS +=  \
	-L../../lib  \

include ($$PWD/../../crosscompile-support.pri)

include($$PWD/src/src.pri)

RESOURCES += \
    $${TARGET}.qrc \
