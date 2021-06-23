PLUGIN_NAME = shared

lupdate_only {
SOURCES += \
    $$PWD/qml/reports/*.qml \
}

RESOURCES+= \
    $$PWD/qml/reports/qml.qrc \

SRC_DATA_DIR = $$PWD/qml/
DEST_DATA_DIR = $$LIBS_DIR/qml/quickevent/$$PLUGIN_NAME/qml/

unix: $${PLUGIN_NAME}.commands = $(MKDIR) $$DEST_DATA_DIR; rsync -r $$SRC_DATA_DIR $$DEST_DATA_DIR
win32 {
    !isEmpty(GITHUB_ACTIONS) {
        $${PLUGIN_NAME}.commands = $(MKDIR) $$DEST_DATA_DIR; cp -R $$SRC_DATA_DIR/* $$DEST_DATA_DIR
    }
    else {
        $${PLUGIN_NAME}.commands = xcopy $$shell_path($$SRC_DATA_DIR) $$shell_path($$DEST_DATA_DIR) /s /e /y /i
    }
}
export($${PLUGIN_NAME}.commands)
´
