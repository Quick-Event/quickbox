TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11
QT += qml widgets

QE_PLUGIN_SRCDIR = $$PWD/$$PLUGIN_MODULE_NAME

QF_PROJECT_TOP_SRCDIR = $$PWD/../../../..
QF_PROJECT_TOP_BUILDDIR = $$OUT_PWD/../../../../..

message( $$PLUGIN_MODULE_NAME src dir: $$QE_PLUGIN_SRCDIR )
message( PROJECT_TOPDIR: $$QF_PROJECT_TOP_SRCDIR )

win32: LIB_DIR_NAME = bin
else:  LIB_DIR_NAME = lib

DESTDIR = $$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME/qml/quickevent
TARGET  = $${PLUGIN_MODULE_NAME}Plugin

INCLUDEPATH += $$QF_PROJECT_TOP_SRCDIR/libqf/libqfcore/include
INCLUDEPATH += $$QF_PROJECT_TOP_SRCDIR/libqf/libqfqmlwidgets/include
INCLUDEPATH += $$QE_PLUGIN_SRCDIR/src

message(INCLUDEPATH: $$INCLUDEPATH)

LIBS += -lqfcore -lqfqmlwidgets

LIBS += -L$$QF_PROJECT_TOP_BUILDDIR/$$LIB_DIR_NAME
#message( LIBS: $$LIBS )

include ($$QF_PROJECT_TOP_SRCDIR/crosscompile-support.pri)

unix {
	qmlfiles.commands = \
		ln -sfT $$QE_PLUGIN_SRCDIR/qml $$DESTDIR/$$PLUGIN_MODULE_NAME
}
win32 {
	qmlfiles.commands = xcopy $$shell_path($$QE_PLUGIN_SRCDIR/qml) $$shell_path($$DESTDIR) /e/y
}

QMAKE_EXTRA_TARGETS += qmlfiles
POST_TARGETDEPS += qmlfiles

OTHER_FILES += \
    $$QE_PLUGIN_SRCDIR/qml/* \

