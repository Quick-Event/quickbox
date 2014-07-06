message(including $$PWD)

TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11
QT += qml widgets

PLUGIN_MODULE_PATH = qml/qf/qmlwidgets

unix:DESTDIR = $$OUT_PWD/../../lib/$$PLUGIN_MODULE_PATH
win:DESTDIR = $$OUT_PWD/../../bin/$$PLUGIN_MODULE_PATH
TARGET  = qfqmlwidgetsplugin

LIBS +=      \
	-lqfcore  \

win32: LIBS +=  \
	-L../../bin  \

unix: LIBS +=  \
	-L../../lib  \

include ($$PWD/../../crosscompile-support.pri)

unix {
	qmlfiles.commands = ln -sf $$PWD/$$PLUGIN_MODULE_PATH/* $$DESTDIR
}
win {
	qmlfiles.commands = cp -rf $$PWD/$$PLUGIN_MODULE_PATH/* $$DESTDIR
}

QMAKE_EXTRA_TARGETS += qmlfiles
POST_TARGETDEPS += qmlfiles

include (src/src.pri)

OTHER_FILES += \
    $$PLUGIN_MODULE_PATH/*.qml \

QML_IMPORT_PATH += \
    $$OUT_PWD/../lib/qml \

