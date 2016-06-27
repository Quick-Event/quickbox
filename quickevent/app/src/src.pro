message(including $$PWD)

QT += core gui qml widgets

CONFIG += C++11

TEMPLATE = app

QF_PROJECT_TOP_SRCDIR = $$PWD/../../..
QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/../../..

DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/bin
TARGET = quickevent

win32: LIB_DIR_NAME = bin
else:  LIB_DIR_NAME = lib

INCLUDEPATH += \
$$QF_PROJECT_TOP_SRCDIR/libqf/libqfcore/include \
$$QF_PROJECT_TOP_SRCDIR/libqf/libqfqmlwidgets/include \

message(INCLUDEPATH: $$INCLUDEPATH)

LIBS += \
	-L$$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME \

LIBS +=      \
	-lqfcore  \
	-lqfqmlwidgets  \

unix: LIBS +=  \
	-L../../../lib  \
	-Wl,-rpath,\'\$\$ORIGIN/../lib:\$\$ORIGIN/../lib/qml/quickevent\'  \

include ($$QF_PROJECT_TOP_SRCDIR/crosscompile-support.pri)

include($$PWD/src.pri)

OTHER_FILES += \
    ../plugins/qml/Core/*.qml \
    ../plugins/qml/SqlDb/*.qml \
    ../plugins/qml/Event/*.qml \
    ../plugins/qml/Help/*.qml \

QML_IMPORT_PATH += \
    $$QF_PROJECT_TOP_BUILDDIR/lib/qml \
	$$QF_PROJECT_TOP_BUILDDIR/lib/qml/quickevent \

DATA_DIR_NAME = $${TARGET}-data

unix {
	CONFIG(debug, debug|release) {
		# T flag is important, qml symlink in SRC/qml dir is created on second install without it
		datafiles.commands = \
			mkdir -p $$DESTDIR/$$DATA_DIR_NAME && \
			ln -sfT $$PWD/../style $$DESTDIR/$$DATA_DIR_NAME/style
	}
	else {
		datafiles.commands = \
			mkdir -p $$DESTDIR/$$DATA_DIR_NAME && \
			rsync -r $$PWD/../style $$DESTDIR/$$DATA_DIR_NAME/
	}
}
win32 {
	#mkdir not needed for windows
	datafiles.commands = \
		xcopy $$shell_path($$PWD/../style) $$shell_path($$DESTDIR/$$DATA_DIR_NAME/style) /E /Y /I
}

win32:CONFIG(debug, debug|release):CONFIG += console
#CONFIG += console

QMAKE_EXTRA_TARGETS += datafiles
PRE_TARGETDEPS += datafiles

TRANSLATIONS += \
	$${TARGET}.cs_CZ.ts \
