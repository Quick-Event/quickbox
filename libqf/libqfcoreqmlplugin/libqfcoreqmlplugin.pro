message(including $$PWD)

TEMPLATE = lib
CONFIG += plugin
CONFIG += c++11
QT += qml

PLUGIN_MODULE_PATH = qml/qf/core

unix:DESTDIR = $$OUT_PWD/../../lib/$$PLUGIN_MODULE_PATH
win:DESTDIR = $$OUT_PWD/../../bin/$$PLUGIN_MODULE_PATH
TARGET  = qfcoreqmlplugin

LIBS +=      \
	-lqfcore  \

unix: LIBS +=  \
	-L$$OUT_PWD/../../lib  \

unix {
	qmlfiles.commands = ln -sf $$PWD/$$PLUGIN_MODULE_PATH/* $$DESTDIR
}
win {
	qmlfiles.commands = cp -rf $$PWD/$$PLUGIN_MODULE_PATH/* $$DESTDIR
}

QMAKE_EXTRA_TARGETS += qmlfiles
POST_TARGETDEPS += qmlfiles

#-------------------- obsolete ------------------------

#pluginfiles.files += \
#	$$PWD/$$PLUGIN_MODULE_PATH/qmldir \

#qml.files = plugins.qml
#qml.path += $$[QT_INSTALL_EXAMPLES]/qml/qmlextensionplugins
#target.path += $$[QT_INSTALL_EXAMPLES]/qml/qmlextensionplugins/imports/TimeExample
#pluginfiles.path += $$[QT_INSTALL_EXAMPLES]/qml/qmlextensionplugins/imports/TimeExample

#INSTALLS += target qml pluginfiles

OTHER_FILES += \
    qml/* \

include (src/src.pri)

