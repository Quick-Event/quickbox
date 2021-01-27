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
message ( QF_PROJECT_TOP_BUILDDIR == '$$QF_PROJECT_TOP_BUILDDIR' )

isEmpty(QF_PROJECT_TOP_SRCDIR) {
    QF_PROJECT_TOP_SRCDIR = $$PWD/../..
}
message ( QF_PROJECT_TOP_SRCDIR == '$$QF_PROJECT_TOP_SRCDIR' )

DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/bin
message ( DESTDIR: $$DESTDIR )

android {
	LIBS +=      \
		-lnecrolog_$${QT_ARCH}  \
		-lqfcore_$${QT_ARCH}  \
		-lqfqmlwidgets_$${QT_ARCH}
}
else {
	LIBS +=      \
		-lnecrolog  \
		-lqfcore  \
		-lqfqmlwidgets
}

win32: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/bin  \

unix: LIBS +=  \
	-L$$QF_PROJECT_TOP_BUILDDIR/lib  \
	-Wl,-rpath,\'\$\$ORIGIN/../lib\' \

# exception backtrace support
CONFIG(debug, debug|release): unix: QMAKE_LFLAGS += -rdynamic

INCLUDEPATH += $$QF_PROJECT_TOP_SRCDIR/3rdparty/necrolog/include
INCLUDEPATH += $$PWD/../../libqf/libqfcore/include
INCLUDEPATH += $$PWD/../../libqf/libqfqmlwidgets/include
INCLUDEPATH += $$PWD/src

win32:CONFIG(debug, debug|release):CONFIG += console
console: message(CONSOLE)

include($$PWD/src/src.pri)

TRANSLATIONS += \
	$${TARGET}.cs_CZ.ts \
	$${TARGET}.fr_FR.ts \
	$${TARGET}.nb_NO.ts \
	$${TARGET}.nl_BE.ts \
	$${TARGET}.pl_PL.ts \
	$${TARGET}.ru_RU.ts \
	$${TARGET}.uk_UA.ts \
