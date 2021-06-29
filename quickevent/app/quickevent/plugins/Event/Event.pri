message(including plugin $$PWD)

PLUGIN_NAME = Event

include (src/src.pri)

OTHER_FILES += \
        $$PWD/qml/reports/* \

lupdate_only {
SOURCES += \
        $$PWD/qml/*.qml \
	$$PWD/qml/reports/*.qml \
}

SRC_DATA_DIR = $$PWD/qml/
DEST_DATA_DIR = $$LIBS_DIR/qml/quickevent/$$PLUGIN_NAME

copyFiles($$SRC_DATA_DIR, $$DEST_DATA_DIR)

