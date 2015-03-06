message(including plugin $$PWD)

PLUGIN_MODULE_NAME = Logging

include ( ../quickeventplugin.pri )

include (src/src.pri)

RESOURCES += \
    $${PLUGIN_MODULE_NAME}.qrc \
