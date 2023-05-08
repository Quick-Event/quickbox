MY_SUBPROJECT = quickhttpd

message($$MY_SUBPROJECT)

TEMPLATE = app

QT += gui sql network
CONFIG += warn_on qt thread

CONFIG += c++17

TARGET = $$MY_SUBPROJECT

QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/..
message ( QF_PROJECT_TOP_BUILDDIR == '$$QF_PROJECT_TOP_BUILDDIR' )

DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/bin
message ( DESTDIR: $$DESTDIR )

INCLUDEPATH += \
    $$PWD/../3rdparty/necrolog/include \
	$$PWD/../libqf/libqfcore/include \
	$$PWD/../libquickevent/libquickeventcore/include \

LIBS +=      \
    -lnecrolog  \
	-lqfcore  \
	-lsiut  \
	-lquickeventcore  \

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

