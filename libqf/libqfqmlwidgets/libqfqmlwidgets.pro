message("including $$PWD")

QT += widgets qml sql
QT += xml printsupport svg # needed by reports

CONFIG += c++11 hide_symbols

DEFINES += QFQMLWIDGETS_BUILD_DLL

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

LIBS +=      \
	-lqfcore  \

win32: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/bin  \

unix: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/lib  \

include ($$PWD/../../crosscompile-support.pri)

include($$PWD/src/src.pri)

RESOURCES += \
    $$PWD/style/style.qrc \
    $$PWD/images/images.qrc \
    $$PWD/images/flat/flat.qrc

TRANSLATIONS += \
	libqfqmlwidgets.cs_CZ.ts \
	libqfqmlwidgets.pl_PL.ts \
