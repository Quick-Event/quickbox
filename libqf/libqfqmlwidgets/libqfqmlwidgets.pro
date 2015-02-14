message("including $$PWD")

TEMPLATE = lib
TARGET=qfqmlwidgets

isEmpty(QF_PROJECT_TOP_BUILDDIR) {
	QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/../..
}
else {
	message ( QF_PROJECT_TOP_BUILDDIR is not empty and set to $$QF_PROJECT_TOP_BUILDDIR )
	message ( This is obviously done in file $$QF_PROJECT_TOP_SRCDIR/.qmake.conf )
}
message ( QF_PROJECT_TOP_BUILDDIR == '$$QF_PROJECT_TOP_BUILDDIR' )

unix:DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/lib
win32:DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/bin

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
	-L$$QF_PROJECT_TOP_BUILDDIR/bin  \

unix: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/lib  \

include ($$PWD/../../crosscompile-support.pri)

include($$PWD/src/src.pri)

RESOURCES += \
    $${TARGET}.qrc \
