message(including plugin $$PWD)

PLUGIN_NAME = Speaker

include (src/src.pri)

RESOURCES += \
       $$PWD/$${PLUGIN_NAME}.qrc
