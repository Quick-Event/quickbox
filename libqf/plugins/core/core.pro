message(including $$PWD)

PLUGIN_NAME = core

include ( ../qfqmlplugin.pri )

QT += qml sql network

CONFIG += c++17 hide_symbols

INCLUDEPATH += src

include (src/src.pri)

RESOURCES += core.qrc
