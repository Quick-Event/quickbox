message(including $$PWD)

TEMPLATE = lib

CONFIG += plugin
CONFIG += c++11
CONFIG += hide_symbols

QT += qml widgets sql

PLUGIN_MODULE_NAME = qmlwidgets
PLUGIN_MODULE_PATH = qml/qf

unix:DESTDIR = $$OUT_PWD/../../lib
win32:DESTDIR = $$OUT_PWD/../../bin

TARGET  = qf$${PLUGIN_MODULE_NAME}plugin

LIBS +=      \
        -lqfcore  \
        -lqfqmlwidgets  \


LIBS += -L$$DESTDIR

include ($$PWD/../../crosscompile-support.pri)

unix {
	qmlfiles.commands = \
		mkdir -p $$DESTDIR/$$PLUGIN_MODULE_PATH $$escape_expand(\n\t) \
		ln -sf $$PWD/$$PLUGIN_MODULE_PATH/* $$DESTDIR/$$PLUGIN_MODULE_PATH
}
win32 {
	qmlfiles.commands = \
		mkdir -p $$DESTDIR/$$PLUGIN_MODULE_PATH $$escape_expand(\n\t) \
		xcopy $$shell_path($$PWD/$$PLUGIN_MODULE_PATH/*) $$shell_path($$DESTDIR/$$PLUGIN_MODULE_PATH) /e/y
}

QMAKE_EXTRA_TARGETS += qmlfiles
POST_TARGETDEPS += qmlfiles

include (src/src.pri)

OTHER_FILES += \
    $$PLUGIN_MODULE_PATH/*.qml \

QML_IMPORT_PATH += \
    $$OUT_PWD/../lib/qml \

