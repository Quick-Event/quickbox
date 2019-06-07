message(including $$PWD)
QT += core gui qml widgets sql xml

CONFIG += C++11

TEMPLATE = app

TARGET = qsqlmon

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

LIBS +=      \
	-lqfcore  \
	-lqfqmlwidgets  \

win32: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/bin  \

unix: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/lib  \
	-Wl,-rpath,\'\$\$ORIGIN/../lib\' \

# exception backtrace support
CONFIG(debug, debug|release): unix: QMAKE_LFLAGS += -rdynamic

include ($$PWD/../../crosscompile-support.pri)

INCLUDEPATH += $$PWD/../../libqf/libqfcore/include
INCLUDEPATH += $$PWD/../../libqf/libqfqmlwidgets/include
INCLUDEPATH += $$PWD/src

#TRANSLATIONS    = qsqlmon_cz.ts

win32:CONFIG(debug, debug|release):CONFIG += console
console: message(CONSOLE)

include($$PWD/src/src.pri)

TRANSLATIONS += \
	$${TARGET}.cs_CZ.ts \
    $${TARGET}.nb_NO.ts \
