MY_SUBPROJECT = quickshow-qml

message($$MY_SUBPROJECT)

TEMPLATE = app

CONFIG += c++11

QT += qml quick sql

DEFINES +=

QF_PROJECT_TOP_SRCDIR = $$PWD/..

isEmpty(QF_PROJECT_TOP_BUILDDIR) {
	QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/..
}
else {
	message ( QF_PROJECT_TOP_BUILDDIR is not empty and set to $$QF_PROJECT_TOP_BUILDDIR )
	message ( This is obviously done in file $$QF_PROJECT_TOP_SRCDIR/.qmake.conf )
}
message ( QF_PROJECT_TOP_BUILDDIR == '$$QF_PROJECT_TOP_BUILDDIR' )

TARGET = $$MY_SUBPROJECT
DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/bin
message ( DESTDIR: $$DESTDIR )

QML_IMPORT_PATH = $$PWD/quickshow-qml-data/qml

# tohle zajisti, aby pri exception backtrace nasel symboly z aplikace
unix:QMAKE_LFLAGS_APP += -rdynamic

INCLUDEPATH += \
	$$PWD/../libqf/libqfcore/include

win32: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/bin  \

unix: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/lib  \
	-Wl,-rpath,\'\$\$ORIGIN/../lib\' \

LIBS +=      \
	-lqfcore  \
#	-lqfqmlwidgets  \

message(LIBS: $$LIBS)

#win32:CONFIG(debug, debug|release):
CONFIG += console
console: message(CONSOLE)

#RESOURCES    += $${MY_SUBPROJECT}.qrc
#FORMS    +=   \

SOURCES += \
	main.cpp \
	application.cpp \
	appclioptions.cpp \
	model.cpp \

HEADERS += \
	application.h \
	appclioptions.h \
	model.h \

OTHER_FILES += \
	quickshow-qml-data/qml/* \

win32:CONFIG(debug, debug|release):CONFIG += console
#CONFIG += console

PROJECT_TOP_SRCDIR = $$PWD
include ($$QF_PROJECT_TOP_SRCDIR/appdatafiles.pri)
