message(including $$PWD)

TEMPLATE = lib

CONFIG += plugin
CONFIG += c++11
CONFIG += hide_symbols

QT += qml #widgets sql

PLUGIN_MODULE_PATH = qml/qf/qmlreports

unix:DESTDIR = $$OUT_PWD/../../lib/$$PLUGIN_MODULE_PATH
win32:DESTDIR = $$OUT_PWD/../../bin/$$PLUGIN_MODULE_PATH
TARGET  = qfqmlreportsplugin

LIBS +=      \
        -lqfcore  \
        -lqfqmlwidgets  \


win32: LIBS +=  \
	-L../../bin  \

unix: LIBS +=  \
	-L../../lib  \

include ($$PWD/../../crosscompile-support.pri)

unix {
	qmlfiles.commands = ln -sf $$PWD/$$PLUGIN_MODULE_PATH/* $$DESTDIR
}
win32 {
        qmlfiles.commands = xcopy $$shell_path($$PWD/$$PLUGIN_MODULE_PATH) $$shell_path($$DESTDIR) /e/y
}

QMAKE_EXTRA_TARGETS += qmlfiles
POST_TARGETDEPS += qmlfiles

include (src/src.pri)

OTHER_FILES += \
    $$PLUGIN_MODULE_PATH/*.qml \

QML_IMPORT_PATH += \
    $$OUT_PWD/../lib/qml \

