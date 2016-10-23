MY_SUBPROJECT = quickshow

message($$MY_SUBPROJECT)

TEMPLATE = app

QT += gui sql widgets

CONFIG += warn_on qt thread

CONFIG += c++11

# exception backrace support
unix:QMAKE_LFLAGS_APP += -rdynamic

TARGET = $$MY_SUBPROJECT

QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/..
message ( QF_PROJECT_TOP_BUILDDIR == '$$QF_PROJECT_TOP_BUILDDIR' )

DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/bin
message ( DESTDIR: $$DESTDIR )

INCLUDEPATH += $$PWD/../libqf/libqfcore/include

LIBS +=      \
	-lqfcore  \
	-lqfqmlwidgets  \

win32: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/bin  \

unix: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/lib  \
	-Wl,-rpath,\'\$\$ORIGIN/../lib\' \

include ($$PWD/../../crosscompile-support.pri)

message(LIBS: $$LIBS)

win32: CONFIG += console

RC_FILE = $${MY_SUBPROJECT}.rc

include ($$PWD/src/src.pri)

OTHER_FILES += \
