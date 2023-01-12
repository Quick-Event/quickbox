TEMPLATE = app

QT += core sql
QT -= gui

TARGET = qfsqldbfs

#message (QF_PROJECT_TOP_SRCDIR: $$QF_PROJECT_TOP_SRCDIR)
#message (QF_PROJECT_TOP_BUILDDIR: $$QF_PROJECT_TOP_BUILDDIR)

isEmpty(QF_PROJECT_TOP_BUILDDIR) {
	QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/../..
}
else {
	message ( QF_PROJECT_TOP_BUILDDIR is not empty and set to $$QF_PROJECT_TOP_BUILDDIR )
	message ( This is obviously done in file $$QF_PROJECT_TOP_SRCDIR/.qmake.conf )
}
message ( QF_PROJECT_TOP_BUILDDIR == '$$QF_PROJECT_TOP_BUILDDIR' )

DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/bin
message ( DESTDIR: $$DESTDIR )

CONFIG += c++17
CONFIG += console
CONFIG -= app_bundle

unix {
	CONFIG += link_pkgconfig
	PKGCONFIG += fuse
}

INCLUDEPATH += $$PWD/../../libqf/libqfcore/include
INCLUDEPATH += $$PWD/src

win32 {
	QB_LIB_DIR = $$QF_PROJECT_TOP_BUILDDIR/bin
}
else {
	QB_LIB_DIR = $$QF_PROJECT_TOP_BUILDDIR/lib
}

LIBS += \
	-lqfcore \
	-L$$QB_LIB_DIR

unix {
	LIBS += -Wl,-rpath,\'\$\$ORIGIN/../lib\'
}

include ($$PWD/src/src.pri)
