message(including plugin $$PWD)

PLUGIN_NAME = Relays

include (src/src.pri)

RESOURCES += \
      $$PWD/$${PLUGIN_NAME}.qrc

lupdate_only {
SOURCES += \
        $$PWD/qml/*.qml \
}

