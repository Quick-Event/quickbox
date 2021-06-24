PLUGIN_NAME = shared

lupdate_only {
SOURCES += \
    $$PWD/qml/reports/*.qml \
}

RESOURCES+= \
    $$PWD/qml/reports/qml.qrc \

SRC_DATA_DIR = $$PWD/qml/
# differs from other plugins, extra "qml" in the DEST_DATA_DIR
DEST_DATA_DIR = $$LIBS_DIR/qml/quickevent/$$PLUGIN_NAME/qml/

copyFiles($$SRC_DATA_DIR, $$DEST_DATA_DIR)
