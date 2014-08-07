message(including $$PWD)

TEMPLATE = lib

CONFIG += plugin
CONFIG += c++11
CONFIG += hide_symbols

QT += qml sql network

PLUGIN_MODULE_PATH = qml/qf/core

unix:DESTDIR = $$OUT_PWD/../../lib/$$PLUGIN_MODULE_PATH
win32:DESTDIR = $$OUT_PWD/../../bin/$$PLUGIN_MODULE_PATH
TARGET  = qfcoreqmlplugin

LIBS +=      \
	-lqfcore  \

unix: LIBS += -L$$OUT_PWD/../../lib
win32: LIBS += -L$$OUT_PWD/../../bin

include ($$PWD/../../crosscompile-support.pri)

unix {
	qmlfiles.commands = ln -sf $$PWD/$$PLUGIN_MODULE_PATH/* $$DESTDIR
}
win32 {
        qmlfiles.commands = xcopy $$shell_path($$PWD/$$PLUGIN_MODULE_PATH) $$shell_path($$DESTDIR) /e/y
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
    #qml/qf/core/sql/def/* \
    #qml/qf/core/sql/def/private/* \

include (src/src.pri)

RESOURCES += \
    $${TARGET}.qrc \
