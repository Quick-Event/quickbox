message(including plugin $$PWD)

PLUGIN_NAME = CardReader

include (src/src.pri)

RESOURCES += \
      $$PWD/$${PLUGIN_NAME}.qrc

