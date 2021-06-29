message(including plugin $$PWD)

PLUGIN_NAME = Event

include (src/src.pri)

RESOURCES += \
       $$PWD/$${PLUGIN_NAME}.qrc

lupdate_only {
SOURCES += \
        $$PWD/qml/*.qml \
}
