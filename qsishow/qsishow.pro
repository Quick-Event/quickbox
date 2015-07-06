MY_SUBPROJECT = qsishow

message($$MY_SUBPROJECT)

TEMPLATE = app

QT += qml quick

DEFINES +=

isEmpty(QF_PROJECT_TOP_BUILDDIR) {
	QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/../..
}
else {
	message ( QF_PROJECT_TOP_BUILDDIR is not empty and set to $$QF_PROJECT_TOP_BUILDDIR )
	message ( This is obviously done in file $$QF_PROJECT_TOP_SRCDIR/.qmake.conf )
}
message ( QF_PROJECT_TOP_BUILDDIR == '$$QF_PROJECT_TOP_BUILDDIR' )

TARGET = $$MY_SUBPROJECT
DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/bin
message ( DESTDIR: $$DESTDIR )

#QML_IMPORT_PATH = /home/fanda/qt/si/qsishow/divers/qsishow/qml

# tohle zajisti, aby pri exception backtrace nasel symboly z aplikace
unix:QMAKE_LFLAGS_APP += -rdynamic

LIBS +=      \
	-lqfcore  \
	-lqfqmlwidgets  \

win32: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/bin  \

unix: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/lib  \
	-Wl,-rpath,\'\$\$ORIGIN/../lib\' \

INCLUDEPATH += $$PWD/../../libqf/libqfcore/include

message(LIBS: $$LIBS)

#win32:CONFIG(debug, debug|release):
CONFIG += console
console: message(CONSOLE)

#RESOURCES    += $${MY_SUBPROJECT}.qrc
#FORMS    +=   \

SOURCES += \
	main.cpp \
	application.cpp \
	model.cpp \

HEADERS += \
	application.h \
	model.h \

OTHER_FILES += \
	qsishow-data/qml/* \
