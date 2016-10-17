MY_SUBPROJECT = quickshow

message($$MY_SUBPROJECT)

TEMPLATE = app

QT += gui sql widgets

CONFIG += warn_on qt thread

CONFIG += c++11

# exception backrace support
unix:QMAKE_LFLAGS_APP += -rdynamic

TARGET = $$OUT_PWD/../bin/$$MY_SUBPROJECT

INCLUDEPATH += $$PWD/../libqf/libqfcore/include

LIBS +=      \
	-lqfcore  \

win32: LIBS +=  \
	-L$$MY_BUILD_DIR/bin  \

unix: LIBS +=  \
	-L../lib  \
	-Wl,-rpath,\'\$\$ORIGIN/../lib\' \

message(LIBS: $$LIBS)

#win32: CONFIG += console

RC_FILE = $${MY_SUBPROJECT}.rc

include ($$PWD/src/src.pri)

OTHER_FILES += \
