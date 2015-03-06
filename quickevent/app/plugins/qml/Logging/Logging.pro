message(including plugin $$PWD)

PLUGIN_NAME = Logging

include ( ../quickeventqmlplugin.pri )

QT += widgets

include (src/src.pri)

RESOURCES += \
    $${PLUGIN_NAME}.qrc \
