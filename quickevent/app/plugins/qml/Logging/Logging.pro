message(including plugin $$PWD)

PLUGIN_NAME = Logging

include ( ../quickeventqmlplugin.pri )

QT += widgets

CONFIG += c++11 hide_symbols

include (src/src.pri)

RESOURCES += \
    $${PLUGIN_NAME}.qrc \
