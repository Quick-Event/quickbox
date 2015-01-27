TEMPLATE = app

QT += core sql
QT -= gui

TARGET = qfsqldbfs

QF_PROJECT_BUILD_ROOT = $(QF_PROJECT_BUILD_ROOT)
isEmpty(QF_PROJECT_BUILD_ROOT) {
	QF_PROJECT_BUILD_ROOT = $$OUT_PWD/../..
}
DESTDIR = $$QF_PROJECT_BUILD_ROOT/bin
message ( DESTDIR: $$DESTDIR )

CONFIG += C++11
CONFIG += console
CONFIG -= app_bundle

unix {
	CONFIG += link_pkgconfig
	PKGCONFIG += fuse
}

INCLUDEPATH += $$PWD/../../libqf/libqfcore/include
INCLUDEPATH += $$PWD/src

win32 {
	QB_LIB_DIR = $$QF_PROJECT_BUILD_ROOT/bin
}
else {
	QB_LIB_DIR = $$QF_PROJECT_BUILD_ROOT/lib
}

LIBS += \
	-lqfcore \
	-L$$QB_LIB_DIR

unix {
	LIBS += -Wl,-rpath,\'\$\$ORIGIN/../lib\'
}

include ($$PWD/src/src.pri)
