message(including $$PWD)
QT += core gui qml widgets sql xml

CONFIG += C++11

TEMPLATE = app

TARGET = qsqlmon

QF_PROJECT_BUILD_ROOT = $(QF_PROJECT_BUILD_ROOT)
isEmpty(QF_PROJECT_BUILD_ROOT) {
	QF_PROJECT_BUILD_ROOT = $$OUT_PWD/../..
}

DESTDIR = $$QF_PROJECT_BUILD_ROOT/bin
message ( DESTDIR: $$DESTDIR )

LIBS +=      \
	-lqfcore  \
	-lqfqmlwidgets  \

win32: LIBS +=  \
	-L$$QF_PROJECT_BUILD_ROOT/bin  \

unix: LIBS +=  \
	-L$$QF_PROJECT_BUILD_ROOT/lib  \
	-Wl,-rpath,\'\$\$ORIGIN/../lib\' \

include ($$PWD/../../crosscompile-support.pri)

INCLUDEPATH += $$PWD/../../libqf/libqfcore/include
INCLUDEPATH += $$PWD/../../libqf/libqfqmlwidgets/include
INCLUDEPATH += $$PWD/src

#TRANSLATIONS    = qsqlmon_cz.ts

win32:CONFIG(debug, debug|release):CONFIG += console
console: message(CONSOLE)

include($$PWD/src/src.pri)

