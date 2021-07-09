PLUGIN_NAME = shared

RESOURCES += \
      $$PWD/$${PLUGIN_NAME}.qrc

lupdate_only {
SOURCES += \
    $$PWD/qml/*.qml \
}
