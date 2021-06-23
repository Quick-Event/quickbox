PLUGIN_NAME = shared

lupdate_only {
SOURCES += \
    $$PWD/qml/reports/*.qml \
}

RESOURCES+= \
    $$PWD/qml/reports/qml.qrc \

SRC_DATA_DIR = $$PWD/qml
DEST_DATA_DIR = $$LIBS_DIR/qml/quickevent/$$PLUGIN_NAME

unix: $${PLUGIN_NAME}.commands = $(MKDIR) $$DEST_DATA_DIR; $(COPY_DIR) $$SRC_DATA_DIR $$DEST_DATA_DIR
win32: $${PLUGIN_NAME}.commands = $(COPY_DIR) \"$$shell_path($$SRC_DATA_DIR)\" \"$$shell_path($$DEST_DATA_DIR)\"
export($${PLUGIN_NAME}.commands)
´
