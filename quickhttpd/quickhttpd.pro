MY_SUBPROJECT = quickhttpd

message($$MY_SUBPROJECT)

TEMPLATE = app

QT += gui sql
CONFIG += warn_on qt thread

CONFIG += c++11

TARGET = $$MY_SUBPROJECT

QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/..
message ( QF_PROJECT_TOP_BUILDDIR == '$$QF_PROJECT_TOP_BUILDDIR' )

DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/bin
message ( DESTDIR: $$DESTDIR )

INCLUDEPATH += \
	$$PWD/../libqf/libqfcore/include
#	$$PWD/../quickevent/lib/include

LIBS +=      \
	-lqfcore  \
#	-lquickevent  \

win32: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/bin  \

unix: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/lib  \
	-Wl,-rpath,\'\$\$ORIGIN/../lib\' \

CONFIG(debug, debug|release) {
	# exception backtrace support
	unix:QMAKE_LFLAGS += -rdynamic
}

message(LIBS: $$LIBS)

win32: CONFIG += console

include ($$PWD/src/src.pri)

