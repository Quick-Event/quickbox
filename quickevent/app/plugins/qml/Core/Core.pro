message(including plugin $$PWD)

PLUGIN_NAME = Core

include ( ../quickeventqmlplugin.pri )

QT += widgets sql

CONFIG += c++11 hide_symbols

include (src/src.pri)

RESOURCES += \

