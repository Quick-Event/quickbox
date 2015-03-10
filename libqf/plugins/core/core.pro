message(including $$PWD)

PLUGIN_NAME = core

include ( ../qfqmlplugin.pri )

QT += qml sql network

INCLUDEPATH += src

include (src/src.pri)

RESOURCES += core.qrc
