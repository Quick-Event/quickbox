message(including $$PWD)

PLUGIN_NAME = core

include ( ../qfqmlplugin.pri )

QT += qml sql network

include (src/src.pri)

RESOURCES += \
	$$PLUGIN_NAME.qrc
